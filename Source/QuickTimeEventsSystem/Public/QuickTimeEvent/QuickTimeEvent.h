// 

#pragma once

#include "CoreMinimal.h"
#include "Structs/QuickTimeEventStructs.h"
#include "UObject/Object.h"
#include "QuickTimeEvent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class QUICKTIMEEVENTSSYSTEM_API UQuickTimeEvent : public UObject
{
	GENERATED_BODY()
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnQuickTimeEventCompletedSignature, UQuickTimeEvent*, QuickTimeEvent, bool, bSuccess, bool, bPerfect);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuickTimeEventTimeoutSignature, UQuickTimeEvent*, QuickTimeEvent);
	
public:
	UFUNCTION(BlueprintCallable, Category = "Quick Time Event")
	void StartQuickTimeEvent();
	
	UPROPERTY(BlueprintAssignable, Category = "Quick Time Event")
	FOnQuickTimeEventTimeoutSignature OnQuickTimeEventTimedOut;
	
	UPROPERTY(BlueprintAssignable, Category = "Quick Time Event")
	FOnQuickTimeEventCompletedSignature OnQuickTimeEventCompleted;
	
	UFUNCTION(BlueprintCallable, Category = "Quick Time Event")
	const FQuickTimeEventDefinition& GetDefinition() const { return Definition; }
	
	UFUNCTION(BlueprintCallable, Category = "Quick Time Event")
	float GetElapsedTime() const;

	UFUNCTION(BlueprintCallable, Category = "Quick Time Event")
	float GetRemainingTime() const;

	UFUNCTION(BlueprintCallable, Category = "Quick Time Event")
	void Cancel();
	void Complete(bool bSuccess, bool bPerfect);

	UFUNCTION(BlueprintCallable, Category = "Quick Time Event")
	bool IsActive() const { return bIsActive; }
	
	UFUNCTION(BlueprintCallable, Category = "Quick Time Event")
	int32 GetPriority() const { return Priority; }
	
	UFUNCTION(BlueprintCallable, Category = "Quick Time Event")
	bool HasResolved() const { return bHasResolved; }
	
protected:
	UFUNCTION()
	void OnTimeout();
	
	UFUNCTION()
	void OnCompleted(bool bSuccess, bool bPerfect);

private:
	UPROPERTY()
	FQuickTimeEventDefinition Definition;
	
	UPROPERTY()
	int32 Priority;

	FTimerHandle TimeoutTimerHandle;
	
	bool bHasResolved;
	bool bIsActive;
};
