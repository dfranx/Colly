#include <Colly/Rect.h>
#include <algorithm>

namespace cl
{
	Rect::Rect()
	{
		X = Y = Width = Height = 0;
	}
	Rect::Rect(float x, float y, float w, float h)
	{
		X = x;
		Y = y;
		Width = w;
		Height = h;
	}

	bool Rect::Intersects(const Rect& o, Rect& intersection)
	{
		// following code is from: https://github.com/SFML/SFML/blob/247b03172c34f25a808bcfdc49f390d619e7d5e0/include/SFML/Graphics/Rect.inl#L109

		// Compute the min and max of the first rectangle on both axes
		float minX1 = std::min(X, X + Width);
		float maxX1 = std::max(X, X + Width);
		float minY1 = std::min(Y, Y + Height);
		float maxY1 = std::max(Y, Y + Height);


		// Compute the min and max of the second rectangle on both axes
		float minX2 = std::min(o.X, o.X + o.Width);
		float maxX2 = std::max(o.X, o.X + o.Width);
		float minY2 = std::min(o.Y, o.Y + o.Height);
		float maxY2 = std::max(o.Y, o.Y + o.Height);


		// Compute the intersection boundaries
		float interLeft = std::max(minX1, minX2);
		float interTop = std::max(minY1, minY2);
		float interRight = std::min(maxX1, maxX2);
		float interBottom = std::min(maxY1, maxY2);

		
		// If the intersection is valid (positive non zero area), then there is an intersection
		if (interLeft < interRight && interTop < interBottom) {
			intersection = Rect(interLeft, interTop, interRight - interLeft, interBottom - interTop);
			return true;
		}

		intersection = Rect(0, 0, 0, 0);
		return false;
	}
}