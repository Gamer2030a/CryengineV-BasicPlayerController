/* Copyright 2017 - 2020 Crytek GmbH / Crytek Group.All rights reserved.
   Copyright 2022 Parsa dehghani / All rights reserved. */
#include "StdAfx.h"
#include "Player.h"
#include "GamePlugin.h"

#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>

#include <DefaultComponents/Cameras/CameraComponent.h>
#include <DefaultComponents/Input/InputComponent.h>
#include <DefaultComponents/Physics/CharacterControllerComponent.h>
#include <DefaultComponents/Geometry/AdvancedAnimationComponent.h>

namespace
{
	static void RegisterPlayerComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerComponent));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerComponent);
}

void CPlayerComponent::Initialize()
{
    //this void should be in the player.cpp
    m_pCameraComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCameraComponent>();
    m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();
    m_pCharacterController = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCharacterControllerComponent>();
    m_pAdvancedAnimationComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CAdvancedAnimationComponent>();

}

void CPlayerComponent::InitializeInput()
{
    m_pInputComponent->RegisterAction("player", "moveforward", [this](int activationMode, float value) {m_movementDelta.y = value; });
    m_pInputComponent->BindAction("player", "moveforward", eAID_KeyboardMouse, eKI_W);

    m_pInputComponent->RegisterAction("player", "moveback", [this](int activationMode, float value) {m_movementDelta.y = -value; });
    m_pInputComponent->BindAction("player", "moveback", eAID_KeyboardMouse, eKI_S);

    m_pInputComponent->RegisterAction("player", "moveright", [this](int activationMode, float value) {m_movementDelta.x = value; });
    m_pInputComponent->BindAction("player", "moveright", eAID_KeyboardMouse, eKI_D);

    m_pInputComponent->RegisterAction("player", "moveleft", [this](int activationMode, float value) {m_movementDelta.x = -value; });
    m_pInputComponent->BindAction("player", "moveleft", eAID_KeyboardMouse, eKI_A);

    m_pInputComponent->RegisterAction("player", "Slowwalk", [this](int activationMode, float value) 
        {
            if (m_CurrentPlayerState != EPlayerState::Crouching)
            {
                if (activationMode == (int)eAAM_OnPress)
                {
                    m_CurrentPlayerState = EPlayerState::SlowWalk;
                }
                else if (activationMode == eAAM_OnRelease)
                {
                    m_CurrentPlayerState = EPlayerState::Walking;
                }
            }
        });
    m_pInputComponent->BindAction("player", "Slowwalk", eAID_KeyboardMouse, eKI_LCtrl);

    m_pInputComponent->RegisterAction("player", "Sprint", [this](int activationMode, float value) 
        {
            if (m_CurrentPlayerState != EPlayerState::Crouching)
            {
                if (activationMode == (int)eAAM_OnPress)
                {
                    m_CurrentPlayerState = EPlayerState::Sprinting;
                }
                else if (activationMode == eAAM_OnRelease)
                {
                    m_CurrentPlayerState = EPlayerState::Walking;
                }
            }
        });
    m_pInputComponent->BindAction("player", "Sprint", eAID_KeyboardMouse, eKI_LShift);

    m_pInputComponent->RegisterAction("player", "yaw", [this](int activationMode, float value) {m_mouseDeltaRotation.y = -value; });
    m_pInputComponent->BindAction("player", "yaw", eAID_KeyboardMouse, eKI_MouseY);

    m_pInputComponent->RegisterAction("player", "pitch", [this](int activationMode, float value) {m_mouseDeltaRotation.x = -value; });
    m_pInputComponent->BindAction("player", "pitch", eAID_KeyboardMouse, eKI_MouseX);

    m_pInputComponent->RegisterAction("player", "crouch", [this](int activationMode, float value) 
        {
            if (activationMode == (int)eAAM_OnPress)
            {
                if (m_CurrentPlayerState == EPlayerState::Sprinting)
                {
                    m_CurrentPlayerState = EPlayerState::Sliding;

                    //Updating the camera position should later put it in its own function 
                    m_cameraCurrentPos = Lerp(m_cameraDefaultPos, m_cameraDefaultCrouchPos, 1.0f);

                    Vec3 velocity = m_pCharacterController->GetVelocity();
                    velocity.Normalize();
                    m_pCharacterController->ChangeVelocity(velocity * 12, Cry::DefaultComponents::CCharacterControllerComponent::EChangeVelocityMode::Add);

                    Matrix34 camDefaultMatrix;
                    camDefaultMatrix.SetTranslation(m_cameraCurrentPos);
                    camDefaultMatrix.SetRotation33(Matrix33(m_pEntity->GetWorldRotation()));
                    m_pCameraComponent->SetTransformMatrix(camDefaultMatrix);
                }
                else
                {
                m_CurrentPlayerState = EPlayerState::Crouching;

                m_cameraCurrentPos = Lerp(m_cameraDefaultPos, m_cameraDefaultCrouchPos, 1.0f);
                Matrix34 camDefaultMatrix;
                camDefaultMatrix.SetTranslation(m_cameraCurrentPos);
                camDefaultMatrix.SetRotation33(Matrix33(m_pEntity->GetWorldRotation()));
                m_pCameraComponent->SetTransformMatrix(camDefaultMatrix);
                }
            }
            else if (activationMode == eAAM_OnRelease)
            {
                m_CurrentPlayerState = EPlayerState::Walking;
                m_cameraCurrentPos = Lerp(m_cameraDefaultCrouchPos, m_cameraDefaultPos, 1.0f);
                Matrix34 camDefaultMatrix;
                camDefaultMatrix.SetTranslation(m_cameraCurrentPos);
                camDefaultMatrix.SetRotation33(Matrix33(m_pEntity->GetWorldRotation()));
                m_pCameraComponent->SetTransformMatrix(camDefaultMatrix);
            }
        });
    m_pInputComponent->BindAction("player", "crouch", eAID_KeyboardMouse, eKI_C);

    m_pInputComponent->RegisterAction("player", "flashlight", [this](int activationMode, float value) 
        {
        });
    m_pInputComponent->BindAction("player", "flashlight", eAID_KeyboardMouse, eKI_F);

    m_pInputComponent->RegisterAction("player", "jump", [this](int activationMode, float value) 
        {
            if (m_pCharacterController->IsOnGround() == true)
            {
                m_CurrentPlayerState = EPlayerState::Jumping;
                m_pCharacterController->ChangeVelocity(m_jumpVelocity, Cry::DefaultComponents::CCharacterControllerComponent::EChangeVelocityMode::Add);
            }
        });
    m_pInputComponent->BindAction("player", "jump", eAID_KeyboardMouse, eKI_Space);

}

Cry::Entity::EventFlags CPlayerComponent::GetEventMask() const
{
    return Cry::Entity::EEvent::GameplayStarted | Cry::Entity::EEvent::Update | Cry::Entity::EEvent::Reset;
}

void CPlayerComponent::ProcessEvent(const SEntityEvent& event)
{
    switch (event.event)
    {
    case Cry::Entity::EEvent::GameplayStarted:
    {
        InitializeInput();
    }
    break;
    case Cry::Entity::EEvent::Update:
    {
        PlayerMovment();

        if (m_playerHealth <= 0)
        {
            //we will kill the player here and dont allow it to move also and we might put a revive state in + double run should be removed
        }
        else
        {
            Ang3 rotationAngle = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));
            rotationAngle.x += m_mouseDeltaRotation.x * m_rotationSpeed;
            rotationAngle.y = CLAMP(rotationAngle.y + m_mouseDeltaRotation.y * m_rotationSpeed, m_rotationLimitMinPitch, m_rotationLimitMaxPitch);
            rotationAngle.z = 0;
            m_lookOrientation = Quat(CCamera::CreateOrientationYPR(rotationAngle));

            Ang3 yawAngle = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));
            yawAngle.y = 0;
            const Quat finalYaw = Quat(CCamera::CreateOrientationYPR(yawAngle));
            m_pEntity->SetRotation(finalYaw);

            Ang3 pitchAngle = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));
            pitchAngle.x = 0;
            Matrix34 finalCamMatrix;
            finalCamMatrix.SetTranslation(m_pCameraComponent->GetTransformMatrix().GetTranslation());
            Matrix33 camRotation = CCamera::CreateOrientationYPR(pitchAngle);
            finalCamMatrix.SetRotation33(camRotation);
            m_pCameraComponent->SetTransformMatrix(finalCamMatrix);
        }
    }
    break;
    case Cry::Entity::EEvent::Reset:
    {
        m_movementDelta = ZERO;
        m_mouseDeltaRotation = ZERO;
        m_lookOrientation = IDENTITY;
            
        Matrix34 camDefaultMatrix;
        camDefaultMatrix.SetTranslation(m_cameraDefaultPos);
        camDefaultMatrix.SetRotation33(Matrix33(m_pEntity->GetWorldRotation()));
        m_pCameraComponent->SetTransformMatrix(camDefaultMatrix);
    }
    break;

    }
}

void CPlayerComponent::PlayerMovment()
{
    Vec3 velocity = Vec3(m_movementDelta.x, m_movementDelta.y, 0.0f);
    velocity.Normalize();
    float playerMoveSpeed;
    //= m_CurrentPlayerState == EPlayerState::Sprinting ? m_runSpeed : m_walkSpeed;
    switch (m_CurrentPlayerState)
    {
    case EPlayerState::Walking:
        playerMoveSpeed = m_walkSpeed;
        break;
    case EPlayerState::Sprinting:
        playerMoveSpeed = m_runSpeed;
        break;
    case EPlayerState::Crouching:
        playerMoveSpeed = m_crouchSpeed;
        break;
    case EPlayerState::SlowWalk:
        playerMoveSpeed = m_slowWalkSpeed;
        break;
    default:
        playerMoveSpeed = m_walkSpeed;
        break;
    }
    m_pCharacterController->SetVelocity(m_pEntity->GetWorldRotation() * velocity * playerMoveSpeed);
}