// 
#pragma once

#include "CoreMinimal.h"
#include "QuickTimeEventDeveloperSettings.h"
#include "Subsystems/WorldSubsystem.h"
#include "QuickTimeEventSubsystem.generated.h"

class UQuickTimeEvent;

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
	 * QTE are automatically sorted by priority
	 */
	UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
	void RegisterQuickTimeEvent(UQuickTimeEvent* QTE);

	/**
	 * Unregister a QTE task from the subsystem
	 */
	UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
	void UnregisterQuickTimeEvent(UQuickTimeEvent* QTE);

	/**
	 * Try to consume input with the highest priority active QTE
	 * @return True if input was consumed by any QTE
	 */	
	UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
	bool TryConsumeInput(FKey PressedKey);
	
	/**
	 * Check if a specific key is ignored by a QTE
	 */	
	UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
	bool IsKeyIgnored(const UQuickTimeEvent* QTE, const FKey& PressedKey) const;
	
	/**
	 * Check if a specific key is the correct key for a QTE
	 */	
	UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
	bool IsCorrectKey(const UQuickTimeEvent* QTE, const FKey& PressedKey) const;

	/**
	 * Get all currently active QTE QTE
	 */
	UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
	TArray<UQuickTimeEvent*> GetActiveQuickTimeEvents() const;

	/**
	 * Get the number of active QTEs
	 */
	UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
	int32 GetActiveQuickTimeEventCount() const { return ActiveQTEs.Num(); }

	/**
	 * Check if a specific key should be globally ignored
	 */
	UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
	bool IsKeyGloballyIgnored(const FKey& Key) const;

	/**
	 * Cancel all active QTEs
	 */
	UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
	void CancelAllQuickTimeEvents();

protected:
	/** Rebuild sorted task list after registration/priority changes */
	void RebuildSortedQTEs();

private:
	/** All active QTE, sorted by priority (highest first) */
	UPROPERTY()
	TArray<TObjectPtr<UQuickTimeEvent>> ActiveQTEs;

	/** Cached developer settings */
	UPROPERTY()
	const UQuickTimeEventDeveloperSettings* CachedSettings;
};
