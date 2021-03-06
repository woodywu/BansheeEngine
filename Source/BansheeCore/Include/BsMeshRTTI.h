//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsCoreApplication.h"
#include "BsMesh.h"
#include "BsSkeleton.h"
#include "BsMorphShapes.h"
#include "BsCoreThread.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class MeshRTTI : public RTTIType<Mesh, MeshBase, MeshRTTI>
	{
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_REFLPTR(mVertexDesc, 0)
			BS_RTTI_MEMBER_PLAIN(mIndexType, 1)
			BS_RTTI_MEMBER_PLAIN(mUsage, 2)
			BS_RTTI_MEMBER_REFLPTR(mSkeleton, 4)
			BS_RTTI_MEMBER_REFLPTR(mMorphShapes, 5)
		BS_END_RTTI_MEMBERS

		SPtr<MeshData> getMeshData(Mesh* obj) 
		{ 
			SPtr<MeshData> meshData = obj->allocBuffer();

			obj->readData(meshData);
			gCoreThread().submit(true);

			return meshData;
		}

		void setMeshData(Mesh* obj, SPtr<MeshData> meshData) 
		{ 
			obj->mCPUData = meshData;
		}

	public:
		MeshRTTI()
			:mInitMembers(this)
		{
			addReflectablePtrField("mMeshData", 3, &MeshRTTI::getMeshData, &MeshRTTI::setMeshData);
		}

		void onDeserializationEnded(IReflectable* obj, const UnorderedMap<String, UINT64>& params) override
		{
			Mesh* mesh = static_cast<Mesh*>(obj);
			mesh->initialize();
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return Mesh::createEmpty();
		}

		const String& getRTTIName() override
		{
			static String name = "Mesh";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_Mesh;
		}
	};

	/** @} */
	/** @endcond */
}