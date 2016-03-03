#pragma once
#include <cassert>
#include "Resources/ResourceManager.h"

namespace sge
{
	template <typename TAG>
	class Handle
	{

		union
		{
		enum VITTU
		{
			// Sizes to use for bitfields
			MAX_BITS_INDEX = 16,
			MAX_BITS_MAGIC = 16,

			// Size to compare against for asserting deferences
			MAX_INDEX = (1 << MAX_BITS_INDEX) - 1,
			MAX_MAGIC = (1 << MAX_BITS_MAGIC) - 1,
		};

		struct
		{
			unsigned m_Index : MAX_BITS_INDEX;	// Index into resource array
			unsigned m_Magic : MAX_BITS_MAGIC;	// Magic number to check
		}PERKELE;

		unsigned int m_Handle;
		} TOINENVITTU;

		ResourceManager* refManager;
	public:
		template <typename T>
		T* getResource() {
			return static_cast<T*>(refManager->getResource<T>(*this));
		}
		// Creation sets the handle to null for error check purposes.
		// Handle needs to be initialized before it can be properly used.
		Handle() : refManager(nullptr) { TOINENVITTU.m_Handle = 0; }
		Handle(ResourceManager* refManager) : refManager(refManager) { TOINENVITTU.m_Handle = 0; }
		void init(unsigned int index);

		// Methods for managing our Handle
		unsigned int getIndex()		const { return (TOINENVITTU.PERKELE.m_Index); }
		unsigned int getMagic()		const { return (TOINENVITTU.PERKELE.m_Magic); }
		unsigned int getHandle()	const { return (TOINENVITTU.m_Handle); }
		bool isNull()				const { return (!TOINENVITTU.m_Handle); }

		operator unsigned int()		const { return(TOINENVITTU.m_Handle); }
	};

	template <typename TAG>
	void Handle<TAG>::init(unsigned int index)
	{
		// Check if the handle is valid.
		assert(isNull());

		// Check that the index is within allocated range.
		assert(index <= TOINENVITTU.MAX_INDEX);

		static unsigned int s_AutoMagic = 0;
		if (++s_AutoMagic > TOINENVITTU.MAX_MAGIC)
		{
			// Value 0 is reserved for Null handle.
			s_AutoMagic = 1;
		}

		TOINENVITTU.PERKELE.m_Index = index;
		TOINENVITTU.PERKELE.m_Magic = s_AutoMagic;
	}

	// ----------------------------------------------------
	// Overwriting operators for simpler Handle comparisons.
	template <typename TAG>
	inline bool operator != (Handle <TAG> h1, Handle <TAG> h2)
	{
		return (h1.GetHandle() != h2.GetHandle());
	}

	template <typename TAG>
	inline bool operator == (Handle <TAG> h1, Handle <TAG> h2)
	{
		return (h1.GetHandle() == h2.GetHandle());
	}

	// ----------------------------------------------------
	// Retrieving resource
}
