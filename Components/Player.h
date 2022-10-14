// Copyright 2017-2020 Crytek GmbH / Crytek Group. All rights reserved.
// Copyright 2022 Parsa dehghani / All rights reserved.
#pragma once

namespace Cry::DefaultComponents
{
    class CCameraComponent;
    class CInputComponent;
    class CCharacterControllerComponent;
    class CAdvancedAnimationComponent;
    class CProjectorLightComponent; //might not be needed
}

enum class EPlayerState
{
    Walking,
    Sprinting,
    Crouching,
    SlowWalk,
    Sliding,
    Jumping
};

enum class EFlashlightState
{
    On,
    Off,
    Flickering,
    OutOfBattery
};


////////////////////////////////////////////////////////
// Represents a player participating in gameplay
////////////////////////////////////////////////////////
class CPlayerComponent final : public IEntityComponent
{
public:
	CPlayerComponent() = default;
	virtual ~CPlayerComponent() = default;

    virtual void Initialize() override;

    virtual Cry::Entity::EventFlags GetEventMask() const override;
    virtual void ProcessEvent(const SEntityEvent& event) override;

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CPlayerComponent>& desc)
	{
		desc.SetGUID("{63F4C0C6-32AF-4ACB-8FB0-57D45DD14725}"_cry_guid);
        desc.AddMember(&CPlayerComponent::m_crouchSpeed, 'pcs', "playercrouchspeed", "player crouch speed", "Sets the Player crouch speed", ZERO);
        desc.AddMember(&CPlayerComponent::m_slowWalkSpeed, 'psws', "playerslowwalkSpeed", "Player SlowWalkSpeed", "Sets the Player Slow Walk speed", ZERO);
        desc.AddMember(&CPlayerComponent::m_walkSpeed, 'pws', "playerwalkSpeed", "Player WalkSpeed", "Sets the Player Walk speed", ZERO);
        desc.AddMember(&CPlayerComponent::m_runSpeed, 'prs', "playerrunSpeed", "Player RunSpeed", "Sets the Player Run speed", ZERO);
        desc.AddMember(&CPlayerComponent::m_slideSpeed, 'pss', "playerslideSpeed", "Player SlideSpeed", "Sets the Player Slide speed", ZERO);
        desc.AddMember(&CPlayerComponent::m_jumpHeight, 'pjh', "playerjumpHeight", "Player JumpHeight", "Sets the Player jump Height", ZERO);
        desc.AddMember(&CPlayerComponent::m_rotationSpeed, 'pros', "playerrotationspeed", "Player RotationSpeed", "Sets the Player rotation speed", ZERO);
        desc.AddMember(&CPlayerComponent::m_cameraDefaultPos, 'pcp', "playercameraposition", "Player cameraPosition", "Sets the Player camera position", ZERO);
        desc.AddMember(&CPlayerComponent::m_cameraDefaultCrouchPos, 'pccp', "playercrouchcameraposition", "Player crouch cameraPosition", "Sets the Player Crouch camera position", ZERO);
        desc.AddMember(&CPlayerComponent::m_rotationLimitMinPitch, 'cpm', "camerapitchmax", "cameraPitchMax", "Sets the Player camera max pitch", 1.5f);
        desc.AddMember(&CPlayerComponent::m_rotationLimitMaxPitch, 'cpmi', "camerapitchmin", "cameraPitchMin", "Sets the Player camera min pitch", -0.85f);
        desc.AddMember(&CPlayerComponent::m_playerHealth, 'poh', "playeroverallhealth", "playeroverallhealth", "Sets the Player overall health", 100.0f);
	}



private:
    Cry::DefaultComponents::CCameraComponent* m_pCameraComponent;
    Cry::DefaultComponents::CInputComponent* m_pInputComponent;
    Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterController;
    Cry::DefaultComponents::CAdvancedAnimationComponent* m_pAdvancedAnimationComponent;
    Cry::DefaultComponents::CProjectorLightComponent* m_pProjectorLightComponent;

    void InitializeInput();
    void PlayerMovment();

    Quat m_lookOrientation;

    Vec3 m_cameraDefaultPos;
    Vec3 m_cameraDefaultCrouchPos;
    Vec3 m_cameraCurrentPos = m_cameraDefaultPos;

    Vec2 m_movementDelta;
    Vec2 m_mouseDeltaRotation;

    float m_jumpHeight;
    float m_movementSpeed;
    float m_rotationSpeed;
    float m_walkSpeed;
    float m_runSpeed;
    float m_crouchSpeed;
    float m_slowWalkSpeed;
    float m_slideSpeed;
    float m_playerHealth;

    float m_rotationLimitMinPitch;
    float m_rotationLimitMaxPitch;

    bool m_flashLightOn = true;
};

EPlayerState m_CurrentPlayerState;
EFlashlightState m_CurrentFlashlightState;