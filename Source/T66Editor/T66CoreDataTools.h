#pragma once

#include "CoreMinimal.h"

/**
 * Editor-only helpers for DT-first core data.
 * No manual Content Browser setup required.
 */
struct FT66CoreDataTools
{
	/** Creates missing DT assets, wires DA_DataCatalog_Core, validates everything. Returns true on PASS. */
	static bool CreateOrRepairAndValidateCoreData(bool bForceOverwrite);
};
