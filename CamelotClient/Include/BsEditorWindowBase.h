#pragma once

#include "BsEditorPrerequisites.h"
#include <boost/signals/connection.hpp>

namespace BansheeEditor
{
	class EditorWindowBase
	{
	public:
		virtual ~EditorWindowBase();

		virtual void setPosition(CM::INT32 x, CM::INT32 y);
		virtual void setSize(CM::UINT32 width, CM::UINT32 height);

		CM::INT32 getLeft() const;
		CM::INT32 getTop() const;

		CM::UINT32 getWidth() const;
		CM::UINT32 getHeight() const;

		virtual void close();
		void hide();
	protected:
		EditorWindowBase();
		EditorWindowBase(CM::RenderWindowPtr renderWindow);

		CM::HSceneObject mSceneObject;
		BS::HGUIWidget mGUI;
		BS::HCamera mCamera;
		bool mOwnsRenderWindow;

		void construct(CM::RenderWindowPtr renderWindow);
		virtual void initialize();
		virtual void movedOrResized() { }
	private:
		boost::signals::connection mMoveOrResizeConn;
		CM::RenderWindowPtr mRenderWindow;
		
		void movedOrResized(CM::RenderWindow& renderWindow);
	};
}