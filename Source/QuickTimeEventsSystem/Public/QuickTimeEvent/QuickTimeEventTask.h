// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "Structs/QuickTimeEventStructs.h"
#include "QuickTimeEventTask.generated.h"

/**
 * 
 */
UCLASS()
class QUICKTIMEEVENTSSYSTEM_API UQuickTimeEventTask : public UGameplayTask
{
	GENERATED_BODY()
public:
    UQuickTimeEventTask(const FObjectInitializer& ObjectInitializer);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FQuickTimeEventDelegate, bool, bWasPerfect, float, completionTime, bool, bTimedOut);

    /** Delegate called when QTE is successfully completed */
    UPROPERTY(BlueprintAssignable)
    FQuickTimeEventDelegate OnSuccess;

    /** Delegate called when QTE fails (timeout or wrong input) */
    UPROPERTY(BlueprintAssignable)
    FQuickTimeEventDelegate OnFailed;

    /**
     * Create and initialize a QTE task
     * @param TaskOwner The owner of this task (usually PlayerController or Pawn)
     * @param Definition Complete QTE definition
     * @param Priority Task priority (higher values execute first)
     */
    UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent", meta = (BlueprintInternalUseOnly = "true", DefaultToSelf = "TaskOwner"))
    static UQuickTimeEventTask* CreateQuickTimeEventTask(AActor* TaskOwner, const FQuickTimeEventDefinition& Definition, int32 Priority = 0);

    /**
     * Try to resolve this QTE with the given input
     * @return True if input was consumed (success or fail), false if ignored
     */
    bool TryResolveWithInput(FKey PressedKey);

    /** Get elapsed time since QTE started */
    UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
    float GetElapsedTime() const;

    /** Get remaining time until timeout */
    UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
    float GetRemainingTime() const;

    /** Check if current timing would be perfect */
    UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
    bool IsTimingPerfect() const;

    /** Get the QTE definition */
    FORCEINLINE const FQuickTimeEventDefinition& GetDefinition() const { return Definition; }

    /** Get task priority */
    FORCEINLINE int32 GetPriority() const { return Priority; }
    
    UPROPERTY(BlueprintReadOnly, Category = "QuickTimeEvent")
    FQuickTimeEventDefinition Definition;

protected:
    // UGameplayTask interface
    virtual void Activate() override;
    virtual void OnDestroy(bool bInOwnerFinished) override;
    virtual void TickTask(float DeltaTime) override;

private:
    /** Internal success handler */
    void HandleSuccess(bool bWasPerfect);

    /** Internal failure handler */
    void HandleFailed(bool bTimedOut);

    /** Check if a key should be ignored */
    bool ShouldIgnoreKey(FKey Key) const;

    /** Timer callback for timeout */
    void OnTimeout();

    UPROPERTY()
    int32 Priority;

    FTimerHandle TimeoutTimerHandle;
    float StartTime;
    bool bHasResolved;
};
