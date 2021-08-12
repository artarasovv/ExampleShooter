// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

class FShooterItemsModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
