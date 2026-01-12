// 
#pragma once

#include "CoreMinimal.h"
#include "QuickTimeEventDeveloperSettings.h"
#include "Subsystems/WorldSubsystem.h"
#include "QuickTimeEventSubsystem.generated.h"

class UQuickTimeEventTask;

/**
 * World Subsystem managing all active QTE tasks
 * Handles input routing and task registration
 */
UCLASS()
class QUICKTIMEEVENTSSYSTEM_API UQuickTimeEventSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Register a QTE task with the subsystem
	 * Tasks are automatically sorted by priority
	 */
	void RegisterQuickTimeEventTask(UQuickTimeEventTask* Task);

	/**
	 * Unregister a QTE task from the subsystem
	 */
	void UnregisterQuickTimeEventTask(UQuickTimeEventTask* Task);

	/**
	 * Try to consume input with the highest priority active QTE
	 * @return True if input was consumed by any QTE
	 */
	UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
	bool TryConsumeInput(FKey PressedKey);

	/**
	 * Get all currently active QTE tasks
	 */
	UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
	TArray<UQuickTimeEventTask*> GetActiveQuickTimeEvents() const;

	/**
	 * Get the number of active QTEs
	 */
	UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
	int32 GetActiveQuickTimeEventCount() const { return ActiveTasks.Num(); }

	/**
	 * Check if a specific key should be globally ignored
	 */
	bool IsKeyGloballyIgnored(FKey Key) const;

	/**
	 * Cancel all active QTEs
	 */
	UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
	void CancelAllQuickTimeEvents();

protected:
	/** Rebuild sorted task list after registration/priority changes */
	void RebuildSortedTasks();

private:
	/** All active QTE tasks, sorted by priority (highest first) */
	UPROPERTY()
	TArray<TObjectPtr<UQuickTimeEventTask>> ActiveTasks;

	/** Cached developer settings */
	UPROPERTY()
	const UQuickTimeEventDeveloperSettings* CachedSettings;
};
