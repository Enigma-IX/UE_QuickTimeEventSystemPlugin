// 

#include "QuickTimeEventDeveloperSettings.h"

UQuickTimeEventDeveloperSettings::UQuickTimeEventDeveloperSettings()
{
	GlobalIgnoredKeys.Add(EKeys::Escape);
	GlobalIgnoredKeys.Add(EKeys::Pause);
	GlobalIgnoredKeys.Add(EKeys::Tab);
}