#include "data/Rectangle.h"
#include "system/Math.h"
#include "data/Convert.h"

namespace Common {
    const RectangleF RectangleF::Empty;
    const RectangleF RectangleF::MaxValue = RectangleF(-65535.0f, -65535.0f, 65535.0f * 2.0f, 65535.0f * 2.0f);

    RectangleF::RectangleF(float x, float y, float width, float height) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }

    RectangleF::RectangleF(PointF location, SizeF size) {
        this->x = location.x;
        this->y = location.y;
        this->width = size.width;
        this->height = size.height;
    }

    RectangleF::RectangleF(const RectangleF &rect) {
        this->x = rect.x;
        this->y = rect.y;
        this->width = rect.width;
        this->height = rect.height;
    }

    bool RectangleF::isEmpty() const {
        return x == 0.0f && y == 0.0f && width == 0.0f && height == 0.0f;
    }

    String RectangleF::toString() const {
        if (this->operator==(Empty))
            return "Empty";
        else if (this->operator==(Empty))
            return "MaxValue";
        return String::convert("%.0f,%.0f,%.0f,%.0f", x, y, width, height);
    }

    PointF RectangleF::location() const {
        return PointF(x, y);
    }

    void RectangleF::setLocation(const PointF &location) {
        x = location.x;
        y = location.y;
    }

    SizeF RectangleF::size() const {
        return SizeF(width, height);
    }

    void RectangleF::setSize(const SizeF &size) {
        width = size.width;
        height = size.height;
    }

    float RectangleF::left() const {
        return x;
    }

    float RectangleF::top() const {
        return y;
    }

    float RectangleF::right() const {
        return x + width;
    }

    float RectangleF::bottom() const {
        return y + height;
    }

    PointF RectangleF::leftTop() const {
        return location();
    }

    PointF RectangleF::rightTop() const {
        return PointF(right(), top());
    }

    PointF RectangleF::leftBottom() const {
        return PointF(left(), bottom());
    }

    PointF RectangleF::rightBottom() const {
        return PointF(right(), bottom());
    }

    void RectangleF::operator=(const RectangleF &value) {
        this->x = value.x;
        this->y = value.y;
        this->width = value.width;
        this->height = value.height;
    }

    bool RectangleF::operator==(const RectangleF &value) const {
        return x == value.x && y == value.y && width == value.width && height == value.height;
    }

    bool RectangleF::operator!=(const RectangleF &value) const {
        return !operator==(value);
    }

    bool RectangleF::contains(float x, float y) const {
        return x >= this->left() &&
               x <= this->right() &&
               y >= this->top() &&
               y <= this->bottom();
    }

    bool RectangleF::contains(const PointF &point) const {
        return contains(point.x, point.y);
    }

    bool RectangleF::contains(const RectangleF &rect) const {
        return (this->x <= rect.x) &&
               ((rect.x + rect.width) <= (this->x + this->width)) &&
               (this->y <= rect.y) &&
               ((rect.y + rect.height) <= (this->y + this->height));
    }

    void RectangleF::inflate(float width, float height) {
        this->x -= width;
        this->y -= height;
        this->width += 2 * width;
        this->height += 2 * height;
    }

    void RectangleF::inflate(const SizeF &size) {
        inflate(size.width, size.height);
    }

    RectangleF RectangleF::intersect(const RectangleF &a, const RectangleF &b) {
        float x1 = Math::max(a.x, b.x);
        float x2 = Math::min(a.x + a.width, b.x + b.width);
        float y1 = Math::max(a.y, b.y);
        float y2 = Math::min(a.y + a.height, b.y + b.height);

        if (x2 >= x1 && y2 >= y1) {
            return RectangleF(x1, y1, x2 - x1, y2 - y1);
        }
        return RectangleF::Empty;
    }

    void RectangleF::intersect(const RectangleF &rect) {
        RectangleF result = RectangleF::intersect(rect, *this);

        this->x = result.x;
        this->y = result.y;
        this->width = result.width;
        this->height = result.height;
    }

    bool RectangleF::intersectsWith(const RectangleF &rect) const {
        return (rect.x < this->x + this->width) &&
               (this->x < (rect.x + rect.width)) &&
               (rect.y < this->y + this->height) &&
               (this->y < rect.y + rect.height);
    }

    RectangleF RectangleF::unions(const RectangleF &a, const RectangleF &b) {
        if (a.size().isEmpty())
            return b;
        if (b.size().isEmpty())
            return a;

        float x1 = Math::min(a.x, b.x);
        float x2 = Math::max(a.x + a.width, b.x + b.width);
        float y1 = Math::min(a.y, b.y);
        float y2 = Math::max(a.y + a.height, b.y + b.height);

        return RectangleF(x1, y1, x2 - x1, y2 - y1);
    }

    void RectangleF::offset(const PointF &pos) {
        offset(pos.x, pos.y);
    }

    void RectangleF::offset(float dx, float dy) {
        this->x += dx;
        this->y += dy;
    }

    void RectangleF::unions(const RectangleF &rect) {
        RectangleF value = unions(*this, rect);
        this->operator=(value);
    }

    bool RectangleF::parse(const String &str, RectangleF &rect) {
        StringArray texts;
        Convert::splitStr(str, ',', texts);
        if (texts.count() == 4) {
            RectangleF temp;
            if (Float::parse(texts[0].trim(), temp.x) &&
                Float::parse(texts[1].trim(), temp.y) &&
                Float::parse(texts[2].trim(), temp.width) &&
                Float::parse(texts[3].trim(), temp.height)) {
                rect = temp;
                return true;
            }
        }
        return false;
    }

    Rectangle RectangleF::round() const {
        return Rectangle((int) Math::round(x), (int) Math::round(y), (int) Math::round(width),
                         (int) Math::round(height));
    }

    void RectangleF::empty() {
        this->operator=(Empty);
    }

    PointF RectangleF::center() const {
        if (size().isEmpty())
            return location();

        return PointF(x + width / 2.0f, y + height / 2.0f);
    }

    RectangleF RectangleF::makeLTRB(float left, float top, float right, float bottom) {
        RectangleF rect(left, top, right - left, bottom - top);
        return rect;
    }

    RectangleF RectangleF::makeLTRB(const PointF &leftTop, const PointF &rightBottom) {
        return makeLTRB(leftTop.x, leftTop.y, rightBottom.x, rightBottom.y);
    }

    const Rectangle Rectangle::Empty;
    const Rectangle Rectangle::MaxValue = Rectangle(-65535, -65535, 65535 * 2, 65535 * 2);

    Rectangle::Rectangle(int x, int y, int width, int height) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }

    Rectangle::Rectangle(Point location, Size size) {
        this->x = location.x;
        this->y = location.y;
        this->width = size.width;
        this->height = size.height;
    }

    Rectangle::Rectangle(const Rectangle &rect) {
        this->x = rect.x;
        this->y = rect.y;
        this->width = rect.width;
        this->height = rect.height;
    }

    bool Rectangle::isEmpty() const {
        return x == 0 && y == 0 && width == 0 && height == 0;
    }

    String Rectangle::toString() const {
        return String::convert("%d,%d,%d,%d", x, y, width, height);
    }

    Point Rectangle::location() const {
        return Point(x, y);
    }

    void Rectangle::setLocation(const Point &location) {
        x = location.x;
        y = location.y;
    }

    Size Rectangle::size() const {
        return Size(width, height);
    }

    void Rectangle::setSize(const Size &size) {
        width = size.width;
        height = size.height;
    }

    int Rectangle::left() const {
        return x;
    }

    int Rectangle::top() const {
        return y;
    }

    int Rectangle::right() const {
        return x + width;
    }

    int Rectangle::bottom() const {
        return y + height;
    }

    void Rectangle::operator=(const Rectangle &value) {
        this->x = value.x;
        this->y = value.y;
        this->width = value.width;
        this->height = value.height;
    }

    bool Rectangle::operator==(const Rectangle &value) const {
        return x == value.x && y == value.y && width == value.width && height == value.height;
    }

    bool Rectangle::operator!=(const Rectangle &value) const {
        return !operator==(value);
    }

    bool Rectangle::contains(int x, int y) const {
        return this->x <= x &&
               x < this->x + this->width &&
               this->y <= y &&
               y < this->y + this->height;
    }

    bool Rectangle::contains(const Point &point) const {
        return contains(point.x, point.y);
    }

    bool Rectangle::contains(const Rectangle &rect) const {
        return (this->x <= rect.x) &&
               ((rect.x + rect.width) <= (this->x + this->width)) &&
               (this->y <= rect.y) &&
               ((rect.y + rect.height) <= (this->y + this->height));
    }

    void Rectangle::inflate(int width, int height) {
        this->x -= width;
        this->y -= height;
        this->width += 2 * width;
        this->height += 2 * height;
    }

    void Rectangle::inflate(const Size &size) {
        inflate(size.width, size.height);
    }

    Rectangle Rectangle::intersect(const Rectangle &a, const Rectangle &b) {
        int x1 = Math::max(a.x, b.x);
        int x2 = Math::min(a.x + a.width, b.x + b.width);
        int y1 = Math::max(a.y, b.y);
        int y2 = Math::min(a.y + a.height, b.y + b.height);

        if (x2 >= x1 && y2 >= y1) {
            return Rectangle(x1, y1, x2 - x1, y2 - y1);
        }
        return Rectangle::Empty;
    }

    void Rectangle::intersect(const Rectangle &rect) {
        Rectangle result = Rectangle::intersect(rect, *this);

        this->x = result.x;
        this->y = result.y;
        this->width = result.width;
        this->height = result.height;
    }

    bool Rectangle::intersectsWith(const Rectangle &rect) const {
        return (rect.x < this->x + this->width) &&
               (this->x < (rect.x + rect.width)) &&
               (rect.y < this->y + this->height) &&
               (this->y < rect.y + rect.height);
    }

    Rectangle Rectangle::unions(const Rectangle &a, const Rectangle &b) {
        if (a.size().isEmpty())
            return b;
        if (b.size().isEmpty())
            return a;

        int x1 = Math::min(a.x, b.x);
        int x2 = Math::max(a.x + a.width, b.x + b.width);
        int y1 = Math::min(a.y, b.y);
        int y2 = Math::max(a.y + a.height, b.y + b.height);

        return Rectangle(x1, y1, x2 - x1, y2 - y1);
    }

    void Rectangle::offset(const Point &pos) {
        offset(pos.x, pos.y);
    }

    void Rectangle::offset(int dx, int dy) {
        this->x += dx;
        this->y += dy;
    }

    void Rectangle::unions(const Rectangle &rect) {
        Rectangle value = unions(*this, rect);
        this->operator=(value);
    }

    bool Rectangle::parse(const String &str, Rectangle &rect) {
        StringArray texts;
        Convert::splitStr(str, ',', texts);
        if (texts.count() == 4) {
            Rectangle temp;
            if (Int32::parse(texts[0].trim(), temp.x) &&
                Int32::parse(texts[1].trim(), temp.y) &&
                Int32::parse(texts[2].trim(), temp.width) &&
                Int32::parse(texts[3].trim(), temp.height)) {
                rect = temp;
                return true;
            }
        }
        return false;
    }

    Point Rectangle::center() const {
        if (size().isEmpty())
            return location();

        return Point(x + width / 2, y + height / 2);
    }

    void Rectangle::empty() {
        this->operator=(Empty);
    }

    Rectangle Rectangle::makeLTRB(int left, int top, int right, int bottom) {
        Rectangle rect(left, top, right - left, bottom - top);
        return rect;
    }
}
