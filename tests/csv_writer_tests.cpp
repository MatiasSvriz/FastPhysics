
#include "fastphysics/csv_writer.hpp"
#include "test_helpers.hpp"

#include <sstream>
#include <string>
#include <vector>

int main()
{
    // -----------------------------------------------------------------------
    // Test 1: Trajectory CSV
    // -----------------------------------------------------------------------

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

        // Write CSV header.
        fastphysics::write_csv_header(
            output
        );

        // Write particle snapshot at t = 0.5.
        fastphysics::write_csv_snapshot(
            output,
            0.5,
            particles
        );

        const std::string expected =
            "time,particle,x,y,z,vx,vy,vz\n"
            "0.5,0,1,2,3,4,5,6\n";

        // Verify exact CSV output.
        FP_CHECK(
            output.str() == expected
        );
    }

    // -----------------------------------------------------------------------
    // Test 2: Energy CSV
    // -----------------------------------------------------------------------

    {
        std::ostringstream output;

        // Write energy CSV header.
        fastphysics::write_energy_csv_header(
            output
        );

        // Write energy snapshot at t = 0.5.
        fastphysics::write_energy_csv_snapshot(
            output,
            0.5,
            0.25,
            -0.5,
            -0.25,
            0.001
        );

        const std::string expected =
            "time,kinetic,potential,total,relative_error\n"
            "0.5,0.25,-0.5,-0.25,0.001\n";

        // Verify exact CSV output.
        FP_CHECK(
            output.str() == expected
        );
    }

    return 0;
}
