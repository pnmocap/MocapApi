// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MocapApi.h"
#include "AvatarActor.generated.h"


UCLASS()
class UE4_MOCAP_API AAvatarActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAvatarActor();

	template<typename ComponentType>
	ComponentType* FindComponent(FString name, bool bIncludeFromChildActors = false)
	{
        auto component_ = FindComponent(ComponentType::StaticClass(),
            name, bIncludeFromChildActors);
        if (nullptr != component_) {
            return StaticCast<ComponentType*>(component_);
        }
        else {
            return nullptr;
        }
	}

	UActorComponent* FindComponent(TSubclassOf<UActorComponent> ComponentClass, 
        FString name, bool bIncludeFromChildActors = false) 
	{
        TArray<UActorComponent*> actorComponents;
        GetComponents(ComponentClass, actorComponents, bIncludeFromChildActors);
        UActorComponent* rComponent_ = nullptr;
        for (auto component_ : actorComponents) {
            if (component_->GetName() == name) {
                rComponent_ = component_;
                break;
            }
        }
        return rComponent_;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
    MocapApi::MCPApplicationHandle_t _appcliation;

private:
    void updateAvatarTransform(MocapApi::MCPAvatarHandle_t avatar);

    void updateJointsTransform(MocapApi::MCPJointHandle_t joint);

};
