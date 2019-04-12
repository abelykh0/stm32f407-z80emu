#include "ay3-8912-state.h"

namespace Sound
{

void Ay3_8912_state::selectRegister(uint8_t registerNumber)
{
	this->selectedRegister = registerNumber;
}

void Ay3_8912_state::setRegisterData(uint8_t data)
{
	switch (this->selectedRegister)
	{
	case 0:
		this->finePitchChannelA = data;
		break;
	case 1:
		this->coarsePitchChannelA = data;
		break;
	case 2:
		this->finePitchChannelB = data;
		break;
	case 3:
		this->coarsePitchChannelB = data;
		break;
	case 4:
		this->finePitchChannelC = data;
		break;
	case 5:
		this->coarsePitchChannelC = data;
		break;
	case 6:
		this->noisePitch = data;
		break;
	case 7:
		this->mixer = data;
		break;
	case 8:
		this->volumeChannelA = data;
		break;
	case 9:
		this->volumeChannelB = data;
		break;
	case 10:
		this->volumeChannelC = data;
		break;
	case 11:
		this->envelopeFineDuration = data;
		break;
	case 12:
		this->envelopeCoarseDuration = data;
		break;
	case 13:
		this->envelopeShape = data;
		break;
	case 14:
		this->ioPortA = data;
		break;
	}
}

uint8_t Ay3_8912_state::getRegisterData()
{
	switch (this->selectedRegister)
	{
	case 0:
		return this->finePitchChannelA;
	case 1:
		return this->coarsePitchChannelA;
	case 2:
		return this->finePitchChannelB;
	case 3:
		return this->coarsePitchChannelB;
	case 4:
		return this->finePitchChannelC;
	case 5:
		return this->coarsePitchChannelC;
	case 6:
		return this->noisePitch;
	case 7:
		return this->mixer;
	case 8:
		return this->volumeChannelA;
	case 9:
		return this->volumeChannelB;
	case 10:
		return this->volumeChannelC;
	case 11:
		return this->envelopeFineDuration;
	case 12:
		return this->envelopeCoarseDuration;
	case 13:
		return this->envelopeShape;
	case 14:
		return this->ioPortA;
	default:
		return 0;
	}
}

}
