#pragma once

#include "Smg/Wrappers.hpp"
#include "Smg/Graph.hpp"
#include "SmgPrinter.hpp"
#include "SmgCrawler.hpp"

#include "OsUtils.hpp"

void ShowSmg(Smg::Impl::Graph& g, const char* viewer = "explorer");

void ShowSmg(Smg::Impl::Graph& g, const char* viewer)
{
  SmgPrinter printer{};
  SmgCrawler crawler{printer};
  Smg::Graph{g}.Accept(crawler);
  auto plot_string = printer.GetDot();

  std::string temp = OsUtils::GetEnv("TEMP");
  std::string dotFileName = temp + PATH_SEPARATOR + "graph.dot";
  std::string svgFileName = temp + PATH_SEPARATOR + "graph.svg";

  std::string command =
    "dot -Tsvg -o" + svgFileName + " -Kdot < " + dotFileName + " && " + viewer + " " + svgFileName;

  OsUtils::WriteToFile(plot_string, dotFileName);
  OsUtils::Exec(command);
}
