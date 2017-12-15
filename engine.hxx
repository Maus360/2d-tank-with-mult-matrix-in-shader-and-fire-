#include <iosfwd>
#include <string>
#include <string_view>

#ifndef eng_DECLSPEC
#define eng_DECLSPEC
#endif

namespace eng
{

/// dendy gamepad emulation events
    enum class event
    {
        /// input events
                left_pressed,
        left_released,
        right_pressed,
        right_released,
        up_pressed,
        up_released,
        down_pressed,
        down_released,
        select_pressed,
        select_released,
        start_pressed,
        start_released,
        button1_pressed,
        button1_released,
        button2_pressed,
        button2_released,
        /// virtual console events
                turn_off
    };

    std::ostream& eng_DECLSPEC operator<<(std::ostream& stream, const event e);

    class engine;

/// return not null on success
    engine* eng_DECLSPEC create_engine();
    void eng_DECLSPEC destroy_engine(engine* e);

    class eng_DECLSPEC color
    {
    public:
        color() = default;
        explicit color(std::uint32_t rgba_);
        color(float r, float g, float b, float a);

        float get_r() const;
        float get_g() const;
        float get_b() const;
        float get_a() const;

        void set_r(const float r);
        void set_g(const float g);
        void set_b(const float b);
        void set_a(const float a);

    private:
        std::uint32_t rgba = 0;
    };

/// position in 2d space
    struct eng_DECLSPEC vec2
    {
        vec2();
        vec2(float x, float y);
        float x = 0.f;
        float y = 0.f;
    };


    vec2 eng_DECLSPEC operator+(const vec2& v1, const vec2& v2);

/// matrix for manipulate coordinates
    struct eng_DECLSPEC mat2x3
    {
        mat2x3();
        static mat2x3 identity();
        static mat2x3 scale(float scal);
        static mat2x3 rotate(float alpha);
        static mat2x3 move(vec2 delta);
        vec2 row1;
        vec2 row2;
        vec2 delta;
    };

    vec2 eng_DECLSPEC operator*(const vec2& mat1, const mat2x3& m);

    vec2 eng_DECLSPEC operator*(const vec2& v, const mat2x3& m);
    mat2x3 eng_DECLSPEC operator*(const mat2x3& m1, const mat2x3& m2);

/// vertex with position only
    struct eng_DECLSPEC v0
    {
        vec2 p;
    };

/// vertex with position and texture coordinate
    struct eng_DECLSPEC v1
    {
        vec2   p;
        color c;
    };

/// vertex position + color + texture coordinate
    struct eng_DECLSPEC v2
    {
        vec2    p;
        vec2 t_p;
        color  c;
    };

/// triangle with positions only
    struct eng_DECLSPEC tri0
    {
        tri0();
        v0 v[3];
    };

/// triangle with positions and color
    struct eng_DECLSPEC tri1
    {
        tri1();
        v1 v[3];
    };

/// triangle with positions color and texture coordinate
    struct eng_DECLSPEC tri2
    {
        tri2();
        v2 v[3];
    };

    std::istream& eng_DECLSPEC operator>>(std::istream& is, color&);
    std::istream& eng_DECLSPEC operator>>(std::istream& is, v0&);
    std::istream& eng_DECLSPEC operator>>(std::istream& is, v1&);
    std::istream& eng_DECLSPEC operator>>(std::istream& is, v2&);
    std::istream& eng_DECLSPEC operator>>(std::istream& is, tri0&);
    std::istream& eng_DECLSPEC operator>>(std::istream& is, tri1&);
    std::istream& eng_DECLSPEC operator>>(std::istream& is, tri2&);

    class eng_DECLSPEC texture
    {
    public:
        virtual ~texture();
        virtual std::uint32_t get_width() const  = 0;
        virtual std::uint32_t get_height() const = 0;
    };

    class eng_DECLSPEC engine
    {
    public:
        virtual ~engine();
        /// create main window
        /// on success return empty string
        virtual std::string initialize(std::string_view config) = 0;
        /// return seconds from initialization
        virtual float get_time_from_init() = 0;
        /// pool event from input queue
        /// return true if more events in queue
        virtual bool read_input(event& e)                      = 0;
        virtual texture* create_texture(std::string_view path) = 0;
        virtual void destroy_texture(texture* t)               = 0;
        virtual void render(const tri0&, const color&) = 0;
        virtual void render(const tri1&) = 0;
        virtual void render(const tri2&, texture*, const mat2x3&) = 0;
        virtual void swap_buffers() = 0;
        virtual void uninitialize() = 0;
    };

} // end namespace eng
