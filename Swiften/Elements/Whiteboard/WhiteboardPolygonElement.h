/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Whiteboard/WhiteboardElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardColor.h>

namespace Swift {
	class WhiteboardPolygonElement : public WhiteboardElement {
		typedef std::pair<int, int> Point;
	public:
		typedef boost::shared_ptr<WhiteboardPolygonElement> ref;
	public:
		WhiteboardPolygonElement() {
		}

		const std::vector<Point>& getPoints() const {
			return points_;
		}

		void setPoints(const std::vector<Point>& points) {
			points_ = points;
		}

		const WhiteboardColor& getPenColor() const {
			return penColor_;
		}

		void setPenColor(const WhiteboardColor& color) {
			penColor_ = color;
		}

		const WhiteboardColor& getBrushColor() const {
			return brushColor_;
		}

		void setBrushColor(const WhiteboardColor& color) {
			brushColor_ = color;
		}

		int getPenWidth() const {
			return penWidth_;
		}

		void setPenWidth(const int penWidth) {
			penWidth_ = penWidth;
		}

		void accept(WhiteboardElementVisitor& visitor) {
			visitor.visit(*this);
		}

	private:
		std::vector<Point> points_;
		WhiteboardColor penColor_;
		WhiteboardColor brushColor_;
		int penWidth_;
	};
}
