// 

#include "QuickTimeEvent/QuickTimeEvent.h"


void UQuickTimeEvent::StartQuickTimeEvent()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("StartQuickTimeEvent: No valid World!"));
		return;
	}
	
	World->GetTimerManager().SetTimer(
		TimeoutTimerHandle,
		this,
		&UQuickTimeEvent::OnTimeout,
		Definition.Settings.Duration,
		false
	);
	bIsActive = true;
	bHasResolved = false;
}

float UQuickTimeEvent::GetElapsedTime() const
{
	if (const UWorld* World = GetWorld())
	{
		return World->GetTimerManager().GetTimerElapsed(TimeoutTimerHandle);
	}
	return 0.0f;
}

float UQuickTimeEvent::GetRemainingTime() const
{
	if (const UWorld* World = GetWorld())
	{
		return World->GetTimerManager().GetTimerRemaining(TimeoutTimerHandle);
	}
	return 0.0f;
}

void UQuickTimeEvent::Cancel()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimeoutTimerHandle);
	}
	bIsActive = false;
}

void UQuickTimeEvent::Complete(bool bSuccess, bool bPerfect)
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimeoutTimerHandle);
	}
	OnCompleted(bSuccess, bPerfect);
}

void UQuickTimeEvent::OnTimeout()
{
	bIsActive = false;
	bHasResolved = false;
	OnQuickTimeEventTimedOut.Broadcast(this);
	OnQuickTimeEventCompleted.Broadcast(this, false, false);
}

void UQuickTimeEvent::OnCompleted(bool bSuccess, bool bPerfect)
{
	bIsActive = false;
	bHasResolved = bSuccess;
	OnQuickTimeEventCompleted.Broadcast(this, bHasResolved, bPerfect);	
}

