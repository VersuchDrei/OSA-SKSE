#pragma once

#include "Util/MCMTable.h"

namespace PapyrusData {
	using VM = RE::BSScript::IVirtualMachine;

	uint32_t GetUndressingSlotMask(RE::StaticFunctionTag*) {
		return MCM::MCMTable::getUndressingMask();
	}

	void SetUndressingSlotMask(RE::StaticFunctionTag*, uint32_t mask) {
		MCM::MCMTable::setUndressingMask(mask);
	}

	bool Bind(VM* a_vm) {
        const auto obj = "OData"sv;

		BIND(GetUndressingSlotMask);
        BIND(SetUndressingSlotMask);

        return true;
	}
}