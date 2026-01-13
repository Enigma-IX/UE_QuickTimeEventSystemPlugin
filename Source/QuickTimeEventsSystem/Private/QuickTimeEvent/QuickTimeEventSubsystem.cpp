// 

#include "QuickTimeEvent/QuickTimeEventSubsystem.h"

#include "EnhancedInputSubsystems.h"
#include "QuickTimeEvent/QuickTimeEvent.h"
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

void UQuickTimeEventSubsystem::RegisterQuickTimeEvent(UQuickTimeEvent* QTE)
{
    if (!QTE)
    {
        return;
    }
    
    // Check if already registered
    if (ActiveQTEs.Contains(QTE))
    {
        UE_LOG(LogTemp, Warning, TEXT("QuickTimeEvent QTE already registered: %s"), 
            *QTE->GetDefinition().QuickTimeEventIdentifier.ToString());
        return;
    }

    ActiveQTEs.Add(QTE);
    RebuildSortedQTEs();
    
    QTE->OnQuickTimeEventTimedOut.AddDynamic(this, &UQuickTimeEventSubsystem::UnregisterQuickTimeEvent);
    
    if (CachedSettings && CachedSettings->bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("QuickTimeEvent QTE Registered: %s | Active Count: %d"),
            *QTE->GetDefinition().QuickTimeEventIdentifier.ToString(),
            ActiveQTEs.Num());
    }
}

void UQuickTimeEventSubsystem::UnregisterQuickTimeEvent(UQuickTimeEvent* QTE)
{
    if (!QTE)
    {
        return;
    }

    int32 RemovedCount = ActiveQTEs.Remove(QTE);
    
    QTE->OnQuickTimeEventTimedOut.RemoveDynamic(this, &UQuickTimeEventSubsystem::UnregisterQuickTimeEvent);

    if (RemovedCount > 0 && CachedSettings && CachedSettings->bEnableDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("QuickTimeEvent QTE Unregistered: %s | Active Count: %d"),
            *QTE->GetDefinition().QuickTimeEventIdentifier.ToString(),
            ActiveQTEs.Num());
    }
}

bool UQuickTimeEventSubsystem::TryConsumeInput(FKey PressedKey)
{
    for (TObjectPtr<UQuickTimeEvent>& QTE : ActiveQTEs)
    {
        UQuickTimeEvent* QTEPtr = QTE.Get();
        if (!QTEPtr)
            continue;

        if (!QTEPtr->IsActive())
            continue;

        if (IsKeyIgnored(QTEPtr, PressedKey))
            continue;

        const bool bCorrectKey = IsCorrectKey(QTEPtr, PressedKey);
        const bool bFailsOnWrongKey = QTEPtr->GetDefinition().InputData.bWrongKeyFails;

        if (!bCorrectKey && !bFailsOnWrongKey)
            continue;

        const float ElapsedTime = QTEPtr->GetElapsedTime();
        const float Duration = QTEPtr->GetDefinition().Settings.Duration;

        const float PerfectMin = Duration * QTEPtr->GetDefinition().Settings.PerfectRangeMin;
        const float PerfectMax = Duration * QTEPtr->GetDefinition().Settings.PerfectRangeMax;

        const bool bPerfect = (ElapsedTime >= PerfectMin && ElapsedTime <= PerfectMax);

        QTEPtr->Complete(bCorrectKey, bPerfect);        
        UnregisterQuickTimeEvent(QTEPtr);

        return true;
    }

    return false;
}

bool UQuickTimeEventSubsystem::IsKeyIgnored(const UQuickTimeEvent* QTE, const FKey& PressedKey) const
{
    if (IsKeyGloballyIgnored(PressedKey))
        return true;
    
    return QTE->GetDefinition().InputData.IgnoredKeys.Contains(PressedKey);
}

bool UQuickTimeEventSubsystem::IsCorrectKey(const UQuickTimeEvent* QTE, const FKey& PressedKey) const
{
    const FQuickTimeEventDefinition& Definition = QTE->GetDefinition();
    
    if (Definition.InputData.InputAction)
    {
        const auto PlayerController = GetWorld()->GetFirstPlayerController();
        const ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr;

        const UInputAction* InputAction = Definition.InputData.InputAction;

        if (LocalPlayer)
        {
            if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<
                UEnhancedInputLocalPlayerSubsystem>())
            {
                TArray<FKey> OutKeys = EnhancedInputSubsystem->QueryKeysMappedToAction(InputAction);
                return OutKeys.Contains(PressedKey);
            }
        }
    }
    
    return Definition.InputData.TargetKey == PressedKey;
}


TArray<UQuickTimeEvent*> UQuickTimeEventSubsystem::GetActiveQuickTimeEvents() const
{
    TArray<UQuickTimeEvent*> Result;
    Result.Reserve(ActiveQTEs.Num());

    for (const TObjectPtr<UQuickTimeEvent>& QTE : ActiveQTEs)
    {
        if (QTE.Get())
        {
            Result.Add(QTE.Get());
        }
    }

    return Result;
}

bool UQuickTimeEventSubsystem::IsKeyGloballyIgnored(const FKey& Key) const
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
    TArray<TObjectPtr<UQuickTimeEvent>> QTEsCopy = ActiveQTEs;

    for (TObjectPtr<UQuickTimeEvent>& QTE : QTEsCopy)
    {
        if (QTE.Get())
        {
            QTE->Cancel();
        }
    }

    ActiveQTEs.Empty();
}

void UQuickTimeEventSubsystem::RebuildSortedQTEs()
{
    Algo::SortBy(
        ActiveQTEs,
        [](const TObjectPtr<UQuickTimeEvent>& QTE)
        {
            UQuickTimeEvent* RawQTE = QTE.Get();
            return RawQTE ? RawQTE->GetPriority() : MIN_int32;
        },
        TGreater<>()
    );
}

