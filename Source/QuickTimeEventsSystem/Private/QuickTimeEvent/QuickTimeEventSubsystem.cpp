// 

#include "QuickTimeEvent/QuickTimeEventSubsystem.h"
#include "QuickTimeEvent/QuickTimeEventTask.h"
#include "QuickTimeEventDeveloperSettings.h"

void UQuickTimeEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Cache settings
    CachedSettings = GetDefault<UQuickTimeEventDeveloperSettings>();

    UE_LOG(LogTemp, Log, TEXT("QuickTimeEvent Subsystem Initialized"));
}

void UQuickTimeEventSubsystem::Deinitialize()
{
    // Cancel all active QuickTimeEvents
    CancelAllQuickTimeEvents();

    Super::Deinitialize();

    UE_LOG(LogTemp, Log, TEXT("QuickTimeEvent Subsystem Deinitialized"));
}

void UQuickTimeEventSubsystem::RegisterQuickTimeEventTask(UQuickTimeEventTask* Task)
{
    if (!Task)
    {
        return;
    }

    // Check if already registered
    if (ActiveTasks.Contains(Task))
    {
        UE_LOG(LogTemp, Warning, TEXT("QuickTimeEvent Task already registered: %s"), 
            *Task->GetDefinition().QuickTimeEventIdentifier.ToString());
        return;
    }

    ActiveTasks.Add(Task);
    RebuildSortedTasks();

    if (CachedSettings && CachedSettings->bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("QuickTimeEvent Task Registered: %s | Active Count: %d"),
            *Task->GetDefinition().QuickTimeEventIdentifier.ToString(),
            ActiveTasks.Num());
    }
}

void UQuickTimeEventSubsystem::UnregisterQuickTimeEventTask(UQuickTimeEventTask* Task)
{
    if (!Task)
    {
        return;
    }

    int32 RemovedCount = ActiveTasks.Remove(Task);

    if (RemovedCount > 0 && CachedSettings && CachedSettings->bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("QuickTimeEvent Task Unregistered: %s | Active Count: %d"),
            *Task->GetDefinition().QuickTimeEventIdentifier.ToString(),
            ActiveTasks.Num());
    }
}

bool UQuickTimeEventSubsystem::TryConsumeInput(FKey PressedKey)
{
    // Try to resolve with the highest priority task first
    for (TObjectPtr<UQuickTimeEventTask>& Task : ActiveTasks)
    {
        if (Task.Get() && Task->TryResolveWithInput(PressedKey))
        {
            return true; // Input was consumed
        }
    }

    return false; // No task consumed the input
}

TArray<UQuickTimeEventTask*> UQuickTimeEventSubsystem::GetActiveQuickTimeEvents() const
{
    TArray<UQuickTimeEventTask*> Result;
    Result.Reserve(ActiveTasks.Num());

    for (const TObjectPtr<UQuickTimeEventTask>& Task : ActiveTasks)
    {
        if (Task.Get())
        {
            Result.Add(Task.Get());
        }
    }

    return Result;
}

bool UQuickTimeEventSubsystem::IsKeyGloballyIgnored(FKey Key) const
{
    if (CachedSettings)
    {
        return CachedSettings->GlobalIgnoredKeys.Contains(Key);
    }
    return false;
}

void UQuickTimeEventSubsystem::CancelAllQuickTimeEvents()
{
    // Create a copy to avoid modification during iteration
    TArray<TObjectPtr<UQuickTimeEventTask>> TasksCopy = ActiveTasks;

    for (TObjectPtr<UQuickTimeEventTask>& Task : TasksCopy)
    {
        if (Task.Get())
        {
            Task->EndTask();
        }
    }

    ActiveTasks.Empty();
}

void UQuickTimeEventSubsystem::RebuildSortedTasks()
{
    Algo::SortBy(
        ActiveTasks,
        [](const TObjectPtr<UQuickTimeEventTask>& Task)
        {
            UQuickTimeEventTask* RawTask = Task.Get();
            return RawTask ? RawTask->GetPriority() : MIN_int32;
        },
        TGreater<>()
    );
}

