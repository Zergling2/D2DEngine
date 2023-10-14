#include "Simplex.h"
#include "Components\CircleCollider.h"
#include "Components\PolygonCollider.h"

using namespace ip;

Simplex::State Simplex::EvolveSimplex()
{
    switch (m_vertices.size())
    {
    case size_t(0):
        m_direction = m_colliderB.m_position - m_colliderA.m_position;
        break;
    case size_t(1):
        m_direction = -m_direction;
        break;
    case size_t(2):
    {
        // line cb is the line formed by the first two vertices
        math::Vector2 cb = m_vertices[1] - m_vertices[0];
        // line c0 is the line from the first vertex to the origin
        math::Vector2 co = -m_vertices[0];

        // use the triple-cross-product to calculate a direction perpendicular to line cb
        // in the direction of the origin
        m_direction = math::Vector2::Cross(math::Vector2::Cross(cb, co), cb);
    }
        break;
    case size_t(3):
    {
        // calculate if the simplex contains the origin
        math::Vector2 ao = -m_vertices[2];                // v2 to the origin
        math::Vector2 ab = m_vertices[1] - m_vertices[2];   // v2 to v1
        math::Vector2 ac = m_vertices[0] - m_vertices[2];   // v2 to v0

        math::Vector2 abPerp = math::Vector2::Cross(math::Vector2::Cross(ac, ab), ab);
        math::Vector2 acPerp = math::Vector2::Cross(math::Vector2::Cross(ab, ac), ac);

        if (math::Vector2::Dot(abPerp, ao) > real(0.0))
        {
            // the origin is outside line ab
            // get rid of c and add a new support in the direction of abPerp
            m_vertices.erase(m_vertices.begin() /* + 0 */);
            m_direction = abPerp;
        }
        else if (math::Vector2::Dot(acPerp, ao) > real(0.0))
        {
            // the origin is outside line ac
            // get rid of b and add a new support in the direction of acPerp
            m_vertices.erase(m_vertices.begin() + 1);
            m_direction = acPerp;
        }
        else
        {
            // the origin is inside both ab and ac,
            // so it must be inside the triangle!
            return Simplex::State::Intersecting;
        }
    }
        break;
    default:
        // case _: throw 'Can\'t have simplex with ${vertices.length} verts!';
        break;
    }

    return AddSupportPoint() ? Simplex::State::Evolving : Simplex::State::NotIntersecting;
}

bool Simplex::AddSupportPoint()
{
    math::Vector2 newVertex = m_colliderA.SupportPoint(m_direction) - m_colliderB.SupportPoint(-m_direction);
    m_vertices.push_back(newVertex);
    return math::Vector2::Dot(m_direction, newVertex) >= real(0.0);
}
