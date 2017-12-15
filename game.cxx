#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>



#include "engine.hxx"

eng::v0 blend(const eng::v0& vl, const eng::v0& vr, const float a)
{
    eng::v0 r;
    r.p.x = (1.0f - a) * vl.p.x + a * vr.p.x;
    r.p.y = (1.0f - a) * vl.p.y + a * vr.p.y;
    return r;
}

eng::tri0 blend(const eng::tri0& tl, const eng::tri0& tr, const float a)
{
    eng::tri0 r;
    r.v[0] = blend(tl.v[0], tr.v[0], a);
    r.v[1] = blend(tl.v[1], tr.v[1], a);
    r.v[2] = blend(tl.v[2], tr.v[2], a);
    return r;
}

int main(int /*argc*/, char* /*argv*/ [])
{
    std::unique_ptr<eng::engine, void (*)(eng::engine*)> engine(
            eng::create_engine(), eng::destroy_engine);

    const std::string error = engine->initialize("");
    if (!error.empty())
    {
        std::cerr << error << std::endl;
        return EXIT_FAILURE;
    }

    eng::texture* texture = engine->create_texture("tank2d.png");
    eng::texture* pula = engine->create_texture("pula.png");

    if (nullptr == texture)
    {
        std::cerr << "failed load texture\n";
        return EXIT_FAILURE;
    }

    bool continue_loop  = true;
    ///angle of main texture ( as default)
    float def = 0.0f;
    ///delta x for pula
    float dx_p = 0.f;
    ///delta y for pula
    float dy_p = 0.f;
    ///bool value for init fire
    bool fire = false;
    ///angle of pula
    float pula_angle = 0.f;
    ///default matrix rotate for 0 degree angle
    eng::mat2x3 rot = eng::mat2x3::rotate(def);
    ///delta x and y for main texture
    float dx = 0.f, dy = 0.f;
    ///default matrix move for 0 and 0 move as x and y
    eng::mat2x3 delta = eng::mat2x3::move(eng::vec2(dx, dy));
    int  current_shader = 0;
    while (continue_loop)
    {
        eng::event event;

        while (engine->read_input(event))
        {
            std::cout << event << std::endl;
            switch (event)
            {
                case eng::event::turn_off:
                    continue_loop = false;
                    break;
                case eng::event::button1_released:
                    ++current_shader;
                    if (current_shader > 2)
                    {
                        current_shader = 0;
                    }
                    break;
                case eng::event::down_pressed:
                    ///check for keeping texture in window
                    if (dx + 0.015f * std::sin(def * M_PI / 180.f) - 0.9f >= 0.0000000001f and
                        ((def >= -90 and def <= 90) or (def <= -270 and def >= 270)))
                        break;
                    if (dy + 0.015f * std::cos(def * M_PI / 180.f) - 0.9f >= 0.0000000001f  and
                        ((def >= 0 and def <= 180) or (def >= -360 and def <= -180)))
                        break;
                    if (dx - 0.015f * std::sin(def * M_PI / 180.f) - -0.9f <= 0.0000000001f  and
                        ((def >= 90 and def <= 270) or (def >= -270 and def <= -90)))
                        break;
                    if (dy - 0.015f * std::cos(def * M_PI / 180.f) - -0.9f <= 0.0000000001f and
                        ((def >= 180 and def <= 360) or (def >= -180 and def <= -0)))
                        break;
                    dx -= static_cast<float>(0.010f * std::sin(def * M_PI / 180.f));
                    dy -= static_cast<float>(0.010f * std::cos(def * M_PI / 180.f));
                    ///generate move matrix with dx and dy
                    delta = eng::mat2x3::move(eng::vec2(dx, dy));
                    break;
                case eng::event::up_pressed:
                    ///check for keeping texture in window
                    if (dx + 0.015f * std::sin(def * M_PI / 180.f) - 0.9f >= 0.0000000001f and
                            ((def >= -90 and def <= 90) or (def <= -270 and def >= 270)))
                        break;
                    if (dy + 0.015f * std::cos(def * M_PI / 180.f) - 0.9f >= 0.0000000001f  and
                            ((def >= 0 and def <= 180) or (def >= -360 and def <= -180)))
                        break;
                    if (dx - 0.015f * std::sin(def * M_PI / 180.f) - -0.9f <= 0.0000000001f  and
                            ((def >= 90 and def <= 270) or (def >= -270 and def <= -90)))
                        break;
                    if (dy - 0.015f * std::cos(def * M_PI / 180.f) - -0.9f <= 0.0000000001f and
                            ((def >= 180 and def <= 360) or (def >= -180 and def <= -0)))
                        break;
                    dx += static_cast<float>(0.015f * std::sin(def * M_PI / 180.f));
                    dy += static_cast<float>(0.015f * std::cos(def * M_PI / 180.f ));
                    ///generate move matrix with dx and dy
                    delta = eng::mat2x3::move(eng::vec2(dx, dy));
                    break;
                case eng::event::left_pressed:
                    ///generate rotate matrix for angle -9 degree
                    rot = eng::mat2x3::rotate(-9.f + def);
                    ///change angle of main texture to def-9 degree
                    def -= 9.f;
                    break;
                case eng::event::right_pressed:
                    ///generate rotate matrix for angle 9 degree
                    rot = eng::mat2x3::rotate(9.f + def);
                    ///change angle of main texture to def+9 degree
                    def += 9.f;
                    break;
                case eng::event::left_released:break;
                case eng::event::right_released:break;
                case eng::event::up_released:break;
                case eng::event::down_released:break;
                case eng::event::select_pressed:break;
                case eng::event::select_released:break;
                case eng::event::start_pressed:break;
                case eng::event::start_released:break;
                case eng::event::button1_pressed:break;
                case eng::event::button2_pressed:
                    if (!fire) {
                        ///set fire is up
                        fire = true;
                        ///set angle of pula as main angle
                        pula_angle = def;
                        ///set dx and dy for pula
                        dy_p = dy;
                        dx_p = dx;
                    }
                    break;
                case eng::event::button2_released:break;
            }
            ///keep main angle in range (-360;360) degree
            if (def >= 360.f) def -= 360.f;
            if (def <= -360.f) def += 360.f;
        }

        if (current_shader == 0)
        {
            std::ifstream file("vert_pos.txt");
            assert(!!file);

            eng::tri0 tr1;
            eng::tri0 tr2;
            eng::tri0 tr11;
            eng::tri0 tr22;

            file >> tr1 >> tr2 >> tr11 >> tr22;

            float time  = engine->get_time_from_init();
            float beta = std::sin(time);

            eng::tri0 t1 = blend(tr1, tr11, beta);
            eng::tri0 t2 = blend(tr2, tr22, beta);

            engine->render(t1, eng::color(1.f, 0.f, 0.f, 1.f));
            engine->render(t2, eng::color(0.f, 1.f, 0.f, 1.f));
        }

        if (current_shader == 1)
        {
            std::ifstream file("vert_pos_color.txt");
            assert(!!file);

            eng::tri1 tr1;
            eng::tri1 tr2;

            file >> tr1 >> tr2;

            engine->render(tr1);
            engine->render(tr2);
        }

        if (current_shader == 2)
        {
            std::ifstream file("vert_tex_color.txt");
            assert(!!file);

            eng::tri2 tr1;
            eng::tri2 tr2;
            eng::tri2 tr3;
            eng::tri2 tr4;

            file >> tr1 >> tr2;
            tr3 = tr1;
            tr4 = tr2;

            // float time = engine->get_time_freng_init();
            // float s    = std::sin(time);
            // float c    = std::sin(time);


            eng::mat2x3 aspect;
            ///matrix for norm coordinates
            aspect.row1.x = 640.f / 480.f;
            aspect.row2.x = 0.f;
            aspect.row1.y = 0.f;
            aspect.row2.y = 1.f;
            ///group rotate scale and move matrixes
            eng::mat2x3 m = aspect * rot * eng::mat2x3::scale(0.25f) * delta;

            engine->render(tr1, texture, m);
            engine->render(tr2, texture, m);
            if (fire) {
                std::cout << pula_angle << std::endl;
                eng::mat2x3 p = aspect * eng::mat2x3::rotate(pula_angle) * eng::mat2x3::scale(0.05)
                                * eng::mat2x3::move(eng::vec2(dx_p, dy_p));
                engine->render(tr3, pula, p);
                engine->render(tr4, pula, p);
                dx_p += static_cast<float>(0.025f * std::sin(pula_angle * M_PI / 180.f));
                dy_p += static_cast<float>(0.025f * std::cos(pula_angle * M_PI / 180.f ));
                if (dx_p >= 1.f or dx_p <= -1.f or dy_p <= -1.f or dy_p >= 1.f){
                    pula_angle = 0.f;
                    fire = false;
                    dx_p = 0.f;
                    dy_p = 0.f;
                }
            }
        }

        engine->swap_buffers();
    }

    engine->uninitialize();

    return EXIT_SUCCESS;
}
