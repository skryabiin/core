#ifndef CORE_CAMERA_FOLOW_FACET_HPP
#define CORE_CAMERA_FOLLOW_FACET_HPP


namespace core {


	struct CameraFollowFacet : public Facet {

		CameraFollowFacet() {
			setDeclaredTypeInfo(&typeid(*this));
		}

	};


} //end namespace core

#endif