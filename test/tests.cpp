#include <catch2/catch_test_macros.hpp>
#include "FrogApp.hpp"

TEST_CASE("FrogApp can be instantiated", "[FrogApp]")
{
  FrogApp app;
  // If we reach here without crashing, the test passes
  REQUIRE(true);
}

TEST_CASE("FrogApp window can be accessed", "[FrogApp]")
{
  FrogApp app;
  const sf::RenderWindow& window = app.getWindow();
  // Verify that we got a valid reference
  REQUIRE(window.isOpen());
}
