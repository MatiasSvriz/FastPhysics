#include "fastphysics/csv_writer.hpp"

#include <cassert>
#include <sstream>
#include <vector>

int main()
{
    const std::vector<fastphysics::Particle> particles{
        {
            {1.0, 2.0, 3.0},
            {4.0, 5.0, 6.0},
            {},
            1.0
        }
    };

    std::ostringstream output;

    fastphysics::write_csv_header(output);

    fastphysics::write_csv_snapshot(
        output,
        0.5,
        particles
    );

    const std::string expected =
        "time,particle,x,y,z,vx,vy,vz\n"
        "0.5,0,1,2,3,4,5,6\n";

    assert(output.str() == expected);
}