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
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "QuickTimeEvent System"))
class QUICKTIMEEVENTSSYSTEM_API UQuickTimeEventDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UQuickTimeEventDeveloperSettings();

	/** Keys that should always be ignored by all QTEs (e.g., Escape, Pause) */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Input")
	TArray<FKey> GlobalIgnoredKeys;

	/** If true, QTE tasks will automatically register/unregister with the subsystem */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "System")
	bool bAutoRegisterTasks = true;

	/** Default timing settings for QTEs when not specified */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	FQuickTimeEventSettings DefaultSettings;

	/** Enable debug logging for QTE system */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bEnableDebugLogging = false;

	/** Show on-screen debug info for active QTEs */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bShowDebugInfo = false;

	// UDeveloperSettings interface
	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
};
