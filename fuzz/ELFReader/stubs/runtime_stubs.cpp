#include <algorithm>
#include <cstring>
#include <string>
#include <unordered_map>

#include "Common/Log.h"
#include "Core/MemMap.h"
#include "Core/Util/BlockAllocator.h"
#include "Core/Debugger/SymbolMap.h"
#include "Core/MIPS/MIPSTables.h"
#include "Core/Debugger/MemBlockInfo.h"

bool g_logEnabled = true;
bool *g_bLogEnabledSetting = &g_logEnabled;
LogChannel g_log[(size_t)Log::NUMBER_OF_LOGS]{};

void GenericLog(Log, LogLevel, const char *, int, const char *, ...) {}

namespace Reporting {
void ReportMessage(const char *, ...) {}
}

namespace Memory {
	u8 *base = nullptr;
	u32 g_MemorySize = 0x02000000;
	u32 g_PSPModel = 0;

	u8 *GetPointerWriteRange(const u32 address, const u32 size) {
		constexpr u32 kBackingSize = 0x10000000;
		if (!base)
			return nullptr;
		const u32 offset = address & 0x3FFFFFFF;
		if (offset >= kBackingSize)
			return nullptr;
		if (size > kBackingSize - offset)
			return nullptr;
		return base + offset;
	}

	Opcode Read_Instruction(const u32, bool) {
		return Opcode(0);
	}

	void Write_U32(const u32 value, u32 address) {
		u8 *ptr = GetPointerWriteRange(address, sizeof(value));
		if (!ptr)
			return;
		std::memcpy(ptr, &value, sizeof(value));
	}
}

void MIPSDisAsm(Memory::Opcode, u32, char *, size_t, bool) {}

std::string StringFromFormat(const char *fmt, ...) {
	return std::string(fmt ? fmt : "");
}

void NotifyMemInfo(MemBlockFlags, u32, u32, const char *, size_t) {}

BlockAllocator userMemory;
BlockAllocator kernelMemory;

namespace {
	struct AllocState {
		bool initialized = false;
		u32 start = 0;
		u32 size = 0;
		u32 nextLow = 0;
		u32 nextHigh = 0;
		bool suballoc = false;
	};

	std::unordered_map<const BlockAllocator *, AllocState> &AllocatorTable() {
		static std::unordered_map<const BlockAllocator *, AllocState> table;
		return table;
	}

	AllocState &State(const BlockAllocator *alloc) {
		return AllocatorTable()[alloc];
	}

	u32 AlignUp(u32 value, u32 grain) {
		if (grain == 0)
			return value;
		return (value + grain - 1) & ~(grain - 1);
	}
}

BlockAllocator::~BlockAllocator() {}

void BlockAllocator::Init(u32 rangeStart, u32 rangeSize, bool suballoc) {
	AllocState &state = State(this);
	state.initialized = true;
	state.start = rangeStart;
	state.size = rangeSize;
	state.nextLow = 0;
	state.nextHigh = rangeSize;
	state.suballoc = suballoc;
}

void BlockAllocator::Shutdown() {
	AllocatorTable().erase(this);
}

void BlockAllocator::ListBlocks() const {
}

u32 BlockAllocator::Alloc(u32 &size, bool fromTop, const char *) {
	AllocState &state = State(this);
	if (!state.initialized)
		return 0xFFFFFFFFu;
	if (size == 0)
		size = grain_;
	size = AlignUp(size, grain_);
	if (size > state.size)
		return 0xFFFFFFFFu;
	if (fromTop) {
		if (size > state.nextHigh)
			return 0xFFFFFFFFu;
		state.nextHigh -= size;
		return state.start + state.nextHigh;
	} else {
		if (state.nextLow + size > state.nextHigh)
			return 0xFFFFFFFFu;
		u32 addr = state.start + state.nextLow;
		state.nextLow += size;
		return addr;
	}
}

u32 BlockAllocator::AllocAligned(u32 &size, u32 sizeGrain, u32 grain, bool fromTop, const char *tag) {
	u32 effectiveGrain = grain ? grain : grain_;
	if (sizeGrain)
		size = AlignUp(size, sizeGrain);
	else
		size = AlignUp(size, effectiveGrain);
	return Alloc(size, fromTop, tag);
}

u32 BlockAllocator::AllocAt(u32 position, u32 size, const char *) {
	AllocState &state = State(this);
	if (!state.initialized)
		return 0xFFFFFFFFu;
	if (size == 0)
		size = grain_;
	u32 end = position + size;
	if (position < state.start)
		return 0xFFFFFFFFu;
	if (end < position)
		return 0xFFFFFFFFu;  // overflow
	if (end > state.start + state.size)
		return 0xFFFFFFFFu;
	// Keep bump pointers ahead of this region so subsequent Alloc() stays out of the way.
	u32 offset = position - state.start;
	state.nextLow = std::max(state.nextLow, offset + size);
	if (state.nextLow > state.size)
		state.nextLow = state.size;
	return position;
}

bool BlockAllocator::Free(u32) { return true; }
bool BlockAllocator::FreeExact(u32) { return true; }

u32 BlockAllocator::GetBlockStartFromAddress(u32 addr) const { return addr; }
u32 BlockAllocator::GetBlockSizeFromAddress(u32) const { return 0; }

u32 BlockAllocator::GetLargestFreeBlockSize() const {
	auto it = AllocatorTable().find(this);
	if (it == AllocatorTable().end())
		return 0;
	const AllocState &state = it->second;
	return state.nextHigh > state.nextLow ? state.nextHigh - state.nextLow : 0;
}

u32 BlockAllocator::GetTotalFreeBytes() const {
	auto it = AllocatorTable().find(this);
	if (it == AllocatorTable().end())
		return 0;
	const AllocState &state = it->second;
	return state.nextHigh > state.nextLow ? state.nextHigh - state.nextLow : 0;
}

const char *BlockAllocator::GetBlockTag(u32) const { return ""; }

void BlockAllocator::DoState(PointerWrap &) {}
void BlockAllocator::CheckBlocks() const {}

BlockAllocator::Block::Block(u32, u32, bool, Block *, Block *) {}
void BlockAllocator::Block::SetAllocated(const char *, bool) {}
void BlockAllocator::Block::DoState(PointerWrap &) {}
void BlockAllocator::MergeFreeBlocks(BlockAllocator::Block *) {}
BlockAllocator::Block *BlockAllocator::GetBlockFromAddress(u32) { return nullptr; }
const BlockAllocator::Block *BlockAllocator::GetBlockFromAddress(u32) const { return nullptr; }
BlockAllocator::Block *BlockAllocator::InsertFreeBefore(BlockAllocator::Block *, u32) { return nullptr; }
BlockAllocator::Block *BlockAllocator::InsertFreeAfter(BlockAllocator::Block *, u32) { return nullptr; }

SymbolMap g_symbolMapInstance;
SymbolMap *g_symbolMap = &g_symbolMapInstance;

void SymbolMap::Clear() {}
void SymbolMap::SortSymbols() {}
void SymbolMap::AddData(u32, u32, DataType, int) {}
void SymbolMap::AddFunction(const char *, u32, u32, int) {}
