// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class AGHSCLONE_API StatusConnection
{
	AActor* unit_with_status;
	bool is_active;

public:

	class AStatusEffect* effect;

	StatusConnection();
	~StatusConnection();

	void SetActive(bool in_is_active)
	{
		is_active = in_is_active;
	}
};
