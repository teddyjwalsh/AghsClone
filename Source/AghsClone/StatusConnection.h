// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class AGHSCLONE_API StatusConnection
{
	bool is_active;
	

public:
	double last_refresh;
	class AStatusEffect* effect;
	AActor* unit_with_status;

	StatusConnection();
	~StatusConnection();

	void SetActive(bool in_is_active)
	{
		is_active = in_is_active;
	}
};
