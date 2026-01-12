// 


#include "QuickTimeEvent/QuickTimeEventTask.h"
#include "QuickTimeEvent/QuickTimeEventSubsystem.h"
#include "QuickTimeEventDeveloperSettings.h"
#include "TimerManager.h"
#include "Engine/World.h"

UQuickTimeEventTask::UQuickTimeEventTask(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , Priority(0)
    , StartTime(0.0f)
    , bHasResolved(false)
{
    bTickingTask = true; // Enable tick for elapsed time tracking
}

UQuickTimeEventTask* UQuickTimeEventTask::CreateQuickTimeEventTask(AActor* TaskOwner, const FQuickTimeEventDefinition& Definition, int32 Priority)
{
    if (!TaskOwner)
    {
        UE_LOG(LogTemp, Error, TEXT("UQuickTimeEventTask::CreateQuickTimeEventTask - TaskOwner is null!"));
        return nullptr;
    }

    UQuickTimeEventTask* NewTask = NewObject<UQuickTimeEventTask>(TaskOwner);
    
    if (NewTask)
    {
        NewTask->Definition = Definition;
        NewTask->Priority = Priority;
        
        // Apply default settings if needed
        if (NewTask->Definition.Settings.Duration <= 0.0f)
        {
            if (const UQuickTimeEventDeveloperSettings* Settings = GetDefault<UQuickTimeEventDeveloperSettings>())
            {
                NewTask->Definition.Settings = Settings->DefaultSettings;
            }
        }
    }

    return NewTask;
}

void UQuickTimeEventTask::Activate()
{
    Super::Activate();

    StartTime = GetWorld()->GetTimeSeconds();
    bHasResolved = false;

    // Register with subsystem
    if (UQuickTimeEventSubsystem* QTESystem = GetWorld()->GetSubsystem<UQuickTimeEventSubsystem>())
    {
        QTESystem->RegisterQuickTimeEventTask(this);
    }

    // Set up timeout timer
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            TimeoutTimerHandle,
            this,
            &UQuickTimeEventTask::OnTimeout,
            Definition.Settings.Duration,
            false
        );
    }

    if (const UQuickTimeEventDeveloperSettings* Settings = GetDefault<UQuickTimeEventDeveloperSettings>())
    {
        if (Settings->bEnableDebugLogging)
        {
            UE_LOG(LogTemp, Log, TEXT("QuickTimeEvent Task Activated: %s, Duration: %.2f, Priority: %d"),
                *Definition.QuickTimeEventIdentifier.ToString(),
                Definition.Settings.Duration,
                Priority);
        }
    }
}

void UQuickTimeEventTask::OnDestroy(bool bInOwnerFinished)
{
    if (UWorld* World = GetWorld())
    {
        // Clear timer
        World->GetTimerManager().ClearTimer(TimeoutTimerHandle);

        // Unregister from subsystem
        if (UQuickTimeEventSubsystem* QTESystem = World->GetSubsystem<UQuickTimeEventSubsystem>())
        {
            QTESystem->UnregisterQuickTimeEventTask(this);
        }
    }

    Super::OnDestroy(bInOwnerFinished);
}

void UQuickTimeEventTask::TickTask(float DeltaTime)
{
    Super::TickTask(DeltaTime);

    // Optional: Add visual feedback or time-based effects here
    if (const UQuickTimeEventDeveloperSettings* Settings = GetDefault<UQuickTimeEventDeveloperSettings>())
    {
        if (Settings->bShowDebugInfo)
        {
            if (GEngine)
            {
                FString DebugInfo = FString::Printf(
                    TEXT("QuickTimeEvent: %s | Time: %.2f/%.2f | Perfect: %s"),
                    *Definition.QuickTimeEventIdentifier.ToString(),
                    GetElapsedTime(),
                    Definition.Settings.Duration,
                    IsTimingPerfect() ? TEXT("YES") : TEXT("NO")
                );
                GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, DebugInfo);
            }
        }
    }
}

bool UQuickTimeEventTask::TryResolveWithInput(FKey PressedKey)
{
    if (bHasResolved)
    {
        return false; // Already resolved
    }

    // Check if key should be ignored
    if (ShouldIgnoreKey(PressedKey))
    {
        return false;
    }

    // Check if it's the correct key
    bool bIsCorrectKey = false;

    // Enhanced Input support
    if (Definition.InputData.InputAction)
    {
        // TODO: Enhanced Input validation requires InputAction context
        // For now, fallback to key comparison
        bIsCorrectKey = (PressedKey == Definition.InputData.TargetKey);
    }
    else
    {
        bIsCorrectKey = (PressedKey == Definition.InputData.TargetKey);
    }

    if (bIsCorrectKey)
    {
        // Success!
        bool bWasPerfect = IsTimingPerfect();
        HandleSuccess(bWasPerfect);
        return true;
    }
    else
    {
        // Wrong key
        if (Definition.InputData.bWrongKeyFails)
        {
            HandleFailed(false); // Failed due to wrong input
            return true; // Input was consumed
        }
        
        return false; // Input ignored
    }
}

float UQuickTimeEventTask::GetElapsedTime() const
{
    if (!GetWorld())
    {
        return 0.0f;
    }
    return GetWorld()->GetTimeSeconds() - StartTime;
}

float UQuickTimeEventTask::GetRemainingTime() const
{
    return FMath::Max(0.0f, Definition.Settings.Duration - GetElapsedTime());
}

bool UQuickTimeEventTask::IsTimingPerfect() const
{
    float NormalizedTime = GetElapsedTime() / Definition.Settings.Duration;
    return NormalizedTime >= Definition.Settings.PerfectRangeMin 
        && NormalizedTime <= Definition.Settings.PerfectRangeMax;
}

void UQuickTimeEventTask::HandleSuccess(bool bWasPerfect)
{
    if (bHasResolved)
    {
        return;
    }

    bHasResolved = true;
    float CompletionTime = GetElapsedTime();

    if (const UQuickTimeEventDeveloperSettings* Settings = GetDefault<UQuickTimeEventDeveloperSettings>())
    {
        if (Settings->bEnableDebugLogging)
        {
            UE_LOG(LogTemp, Log, TEXT("QuickTimeEvent Success: %s | Perfect: %s | Time: %.2f"),
                *Definition.QuickTimeEventIdentifier.ToString(),
                bWasPerfect ? TEXT("YES") : TEXT("NO"),
                CompletionTime);
        }
    }

    OnSuccess.Broadcast(bWasPerfect, CompletionTime, false);
    EndTask();
}

void UQuickTimeEventTask::HandleFailed(bool bTimedOut)
{
    if (bHasResolved)
    {
        return;
    }

    bHasResolved = true;

    if (const UQuickTimeEventDeveloperSettings* Settings = GetDefault<UQuickTimeEventDeveloperSettings>())
    {
        if (Settings->bEnableDebugLogging)
        {
            UE_LOG(LogTemp, Warning, TEXT("QuickTimeEvent Failed: %s | Reason: %s"),
                *Definition.QuickTimeEventIdentifier.ToString(),
                bTimedOut ? TEXT("Timeout") : TEXT("Wrong Input"));
        }
    }

    OnFailed.Broadcast(false, Definition.Settings.Duration, bTimedOut);
    EndTask();
}

bool UQuickTimeEventTask::ShouldIgnoreKey(FKey Key) const
{
    // Check global ignored keys
    if (UQuickTimeEventSubsystem* QTESystem = GetWorld()->GetSubsystem<UQuickTimeEventSubsystem>())
    {
        if (QTESystem->IsKeyGloballyIgnored(Key))
        {
            return true;
        }
    }

    // Check QTE-specific ignored keys
    return Definition.InputData.IgnoredKeys.Contains(Key);
}

void UQuickTimeEventTask::OnTimeout()
{
    HandleFailed(true); // Failed due to timeout
}