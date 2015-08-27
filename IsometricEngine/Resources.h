#pragma once

struct Resource
{
	unsigned char Code;
	unsigned int Amount;

	Resource(unsigned char code, unsigned int amount)
	{
		Code = code;
		Amount = amount;
	}
};

//extern Resource Resources[];