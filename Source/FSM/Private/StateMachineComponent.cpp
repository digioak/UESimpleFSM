// Fill out your copyright notice in the Description page of Project Settings.

#include "StateMachineComponent.h"

UStateMachineComponent::UStateMachineComponent()
{
  PrimaryComponentTick.bCanEverTick = true;
}

void UStateMachineComponent::BeginPlay()
{
  Super::BeginPlay();

  SwitchState(InitialStateTag);
}

void UStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (bCanTickState)
  {
    TickState(DeltaTime);
  }

  if (bDebug)
  {
    GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("FSM: %s is %s"), *GetOwner()->GetName(), *StateTag.ToString()));

    if (StateHistory.Num() > 0)
    {
      for (int32 i = 0; i < StateHistory.Num(); i++)
      {
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, FString::Printf(TEXT("FSM: %s was %s"), *GetOwner()->GetName(), *StateHistory[i].ToString()));
      }
    }
  }
}

bool UStateMachineComponent::SwitchState(FGameplayTag _StateTag)
{
  if (!_StateTag.MatchesTagExact(StateTag))
  {
    bCanTickState = false;
    EndState();
    StateTag = _StateTag;
    InitState();
    bCanTickState = true;
    if (StateChangedDelegate.IsBound())
    {
      StateChangedDelegate.Broadcast(StateTag);
    }
    return true;
  }
  else
  {
    if (bDebug)
    {
      UE_LOG(LogTemp, Warning, TEXT("FSM: %s failed to transition to %s"), *GetOwner()->GetName(), *_StateTag.ToString());
    }
  }
  return false;
}

void UStateMachineComponent::InitState()
{
  if (InitStateDelegate.IsBound())
  {
    InitStateDelegate.Broadcast(StateTag);
  }
}

void UStateMachineComponent::TickState(float DeltaTime)
{
  if (TickStateDelegate.IsBound())
  {
    TickStateDelegate.Broadcast(DeltaTime, StateTag);
  }
}

void UStateMachineComponent::EndState()
{
  if (StateHistory.Num() >= StateHistoryLength)
  {
    StateHistory.RemoveAt(0);
  }

  StateHistory.Push(StateTag);

  if (EndStateDelegate.IsBound())
  {
    EndStateDelegate.Broadcast(StateTag);
  }
}