// Fill out your copyright notice in the Description page of Project Settings.


#include "Hero.h"

AHero::AHero()
{
	attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("attributes"));

	StatInterfaces.Add(attributes);
}