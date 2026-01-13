//

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Structs/QuickTimeEventStructs.h"
#include "QuickTimeEventDeveloperSettings.generated.h"

/**
 * Developer settings for the QTE Plugin
 * Accessible via Project Settings > Plugins > QTE System
 */
UCLASS(config = Game, ProjectUserConfig, meta = (DisplayName = "QuickTimeEvent System"))
class QUICKTIMEEVENTSSYSTEM_API UQuickTimeEventDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UQuickTimeEventDeveloperSettings();

	/** Keys that should always be ignored by all QTEs (e.g., Escape, Pause) */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Input")
	TArray<FKey> GlobalIgnoredKeys;

	/** Enable debug logging for QTE system */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bEnableDebugLogging = true;

	// UDeveloperSettings interface
	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
};
