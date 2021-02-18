// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusConnection.h"

#include "StatusManager.h"

StatusConnection::StatusConnection()
{
}

StatusConnection::~StatusConnection()
{
	effect->RemoveConnection(this);
}
