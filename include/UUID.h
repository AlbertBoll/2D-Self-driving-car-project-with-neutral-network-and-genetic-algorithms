#pragma once

#include <cstdint>
#include<xhash>

class UUID
{

public:

	UUID();
	UUID(uint64_t id);
	UUID(const UUID& other) = default;
	operator uint64_t() const { return m_UUID; }

private:
	uint64_t m_UUID;
};

namespace std
{
	template<>
	struct hash<UUID>
	{
		size_t operator()(const UUID& uuid)const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}