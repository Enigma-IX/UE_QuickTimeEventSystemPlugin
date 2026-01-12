// 

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"

#include "QuickTimeEventStructs.generated.h"

/**
 * Settings for QuickTimeEvent timing and success ranges
 */
USTRUCT(BlueprintType)
struct QUICKTIMEEVENTSSYSTEM_API FQuickTimeEventSettings
{
	GENERATED_BODY()

	/** Duration of the QTE in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickTimeEvent|Timing", meta = (ClampMin = "0.1"))
	float Duration = 2.0f;

	/** Minimum time ratio for perfect timing (0.0 to 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickTimeEvent|Timing", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float PerfectRangeMin = 0.8f;

	/** Maximum time ratio for perfect timing (0.0 to 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickTimeEvent|Timing", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float PerfectRangeMax = 1.0f;

	FQuickTimeEventSettings()
		: Duration(2.0f)
		, PerfectRangeMin(0.8f)
		, PerfectRangeMax(1.0f)
	{}
};

/**
 * Input configuration for QuickTimeEvent
 */
USTRUCT(BlueprintType)
struct QUICKTIMEEVENTSSYSTEM_API FQuickTimeEventInputData
{
	GENERATED_BODY()

	/** The input action required to complete this QTE */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickTimeEvent|Input")
	TObjectPtr<UInputAction> InputAction = nullptr;

	/** Legacy key support (if not using Enhanced Input) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickTimeEvent|Input")
	FKey TargetKey;

	/** If true, any key except ignored keys will fail the QTE. If false, wrong keys are simply ignored */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickTimeEvent|Input")
	bool bWrongKeyFails = true;

	/** Specific keys to ignore for this QTE (won't trigger fail even if bWrongKeyFails is true) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickTimeEvent|Input")
	TArray<FKey> IgnoredKeys;

	FQuickTimeEventInputData()
		: InputAction(nullptr)
		, TargetKey(EKeys::Invalid)
		, bWrongKeyFails(true)
	{}
};

/**
 * Complete QTE definition combining settings and input data
 */
USTRUCT(BlueprintType)
struct QUICKTIMEEVENTSSYSTEM_API FQuickTimeEventDefinition
{
	GENERATED_BODY()

	/** Timing and success range settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickTimeEvent")
	FQuickTimeEventSettings Settings;

	/** Input configuration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickTimeEvent")
	FQuickTimeEventInputData InputData;

	/** Optional identifier for this QTE type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickTimeEvent")
	FName QuickTimeEventIdentifier;

	FQuickTimeEventDefinition()
		: QuickTimeEventIdentifier(NAME_None)
	{}
};

