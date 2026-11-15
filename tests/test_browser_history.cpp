// TEEEEEEEESTS
// interesting fact: a script was used to write a LOT OF repetitive stuff in this code.
// That's because just copypasting wouldnt work since everywhere different numbers must be used.
// Well, anyways... the tests work regardless.

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "../DoublyLinkedList.h"
#include "../BrowserHistory.h"

// NO COMMENTS HERE BECAUSE THESE ARE TESTS 

namespace {

int checksRun = 0;
int checksFailed = 0;

void section(const std::string& name) {
    std::cout << "\n-- " << name << " " << std::string(name.size() < 56 ? 56 - name.size() : 0, '-')
              << std::endl;
}

void check(const std::string& label, bool ok) {
    checksRun++;
    if (!ok) {
        checksFailed++;
    }
    std::cout << (ok ? "  [ PASS ] " : "  [ FAIL ] ") << label << std::endl;
}

void checkEquals(const std::string& label, const std::string& got, const std::string& want) {
    bool ok = (got == want);
    check(label, ok);
    if (!ok) {
        std::cout << "           expected [" << want << "]" << std::endl;
        std::cout << "           actual   [" << got << "]" << std::endl;
    }
}

void checkEquals(const std::string& label, int got, int want) {
    bool ok = (got == want);
    check(label, ok);
    if (!ok) {
        std::cout << "           expected " << want << ", actual " << got << std::endl;
    }
}

std::string snapshot(BrowserHistory& history) {
    std::ostringstream buffer;
    std::streambuf* previous = std::cout.rdbuf(buffer.rdbuf());
    history.display_history();
    std::cout.rdbuf(previous);
    return buffer.str();
}

std::string row(const std::string& title, const std::string& url, bool isCurrent) {
    std::string out = isCurrent ? "  [<] " : "      ";
    if (!title.empty()) {
        out += title + " - ";
    }
    out += url + "\n";
    return out;
}

int pageCount(BrowserHistory& history) {
    std::string text = snapshot(history);
    int lines = 0;
    for (char c : text) {
        if (c == '\n') {
            lines++;
        }
    }
    return lines;
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream out(filename);
    if (out.is_open()) {
        out << content;
    }
}

std::string readFile(const std::string& filename) {
    std::ifstream in(filename);
    std::ostringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

void testNode() {
    section("node class");
    DoublyLinkedList node;
    checkEquals("default url", node.getUrl(), "https://www.youtube.com/watch?v=E4WlUXrJgy4");
    checkEquals("default title", node.getTitle(), "not a rickroll :D");
    check("default next is null", node.getNext() == nullptr);
    check("default prev is null", node.getPrev() == nullptr);

    node.setUrl("https://x.com");
    node.setTitle("X");
    checkEquals("setUrl", node.getUrl(), "https://x.com");
    checkEquals("setTitle", node.getTitle(), "X");

    bool threw = false;
    try {
        node.getNextUrl();
    } catch (const std::out_of_range&) {
        threw = true;
    }
    check("getNextUrl throws past the tail", threw);

    threw = false;
    try {
        node.getPrevUrl();
    } catch (const std::out_of_range&) {
        threw = true;
    }
    check("getPrevUrl throws before the head", threw);

    threw = false;
    try {
        node.getNextTitle();
    } catch (const std::out_of_range&) {
        threw = true;
    }
    check("getNextTitle throws past the tail", threw);

    threw = false;
    try {
        node.getPrevTitle();
    } catch (const std::out_of_range&) {
        threw = true;
    }
    check("getPrevTitle throws before the head", threw);

    DoublyLinkedList first("https://a.com", "A", nullptr, nullptr);
    DoublyLinkedList second("https://b.com", "B", nullptr, &first);
    first.setNext(&second);
    checkEquals("linked getNextUrl", first.getNextUrl(), "https://b.com");
    checkEquals("linked getNextTitle", first.getNextTitle(), "B");
    checkEquals("linked getPrevUrl", second.getPrevUrl(), "https://a.com");
    checkEquals("linked getPrevTitle", second.getPrevTitle(), "A");
}

void testConstruction() {
    section("construction");
    BrowserHistory history("https://google.com", "Google");
    checkEquals("homepage is current", history.current_page(), "https://google.com");
    checkEquals("homepage title", history.current_title(), "Google");
    checkEquals("one page in the list", pageCount(history), 1);
    checkEquals("list contents", snapshot(history), row("Google", "https://google.com", true));

    BrowserHistory untitled("https://no-title.example");
    checkEquals("title defaults to empty", untitled.current_title(), "");
    checkEquals("untitled row prints url only", snapshot(untitled),
                row("", "https://no-title.example", true));
}

void testVisit() {
    section("visit");
    BrowserHistory history("https://google.com", "Google");
    history.visit("https://github.com", "GitHub");
    checkEquals("visit moves current", history.current_page(), "https://github.com");
    checkEquals("visit stores title", history.current_title(), "GitHub");
    checkEquals("two pages", pageCount(history), 2);

    history.visit("https://en.wikipedia.org", "Wikipedia");
    history.visit("https://no-title.example");
    checkEquals("title is optional on visit", history.current_title(), "");
    checkEquals("four pages in order", snapshot(history),
                row("Google", "https://google.com", false) +
                row("GitHub", "https://github.com", false) +
                row("Wikipedia", "https://en.wikipedia.org", false) +
                row("", "https://no-title.example", true));

    BrowserHistory repeat("https://a.com", "A");
    repeat.visit("https://a.com", "A again");
    checkEquals("the same url twice makes two pages", pageCount(repeat), 2);
    checkEquals("the newer title wins", repeat.current_title(), "A again");
}

void testBackOneHundredTimesOnTwoPages() {
    section("back() 100 times on a history of 2 pages");
    BrowserHistory history("https://first.com", "First");
    history.visit("https://second.com", "Second");

    for (int i = 0; i < 100; i++) {
        history.back(1);
    }
    checkEquals("100 single steps back stop at the first page", history.current_page(),
                "https://first.com");
    checkEquals("no pages were lost", pageCount(history), 2);

    BrowserHistory again("https://first.com", "First");
    again.visit("https://second.com", "Second");
    checkEquals("back(100) in one call stops at the first page", again.back(100),
                "https://first.com");
    checkEquals("back(1000000) also stops there", again.back(1000000), "https://first.com");
    checkEquals("forward(100) returns to the last page", again.forward(100),
                "https://second.com");
    checkEquals("forward(100) again stays there", again.forward(100), "https://second.com");
    checkEquals("the list is intact after all that", snapshot(again),
                row("First", "https://first.com", false) +
                row("Second", "https://second.com", true));
}

void testBranchingFromTheMiddle() {
    section("branching history from the middle");
    BrowserHistory history("https://p1.com", "P1");
    history.visit("https://p2.com", "P2");
    history.visit("https://p3.com", "P3");
    history.visit("https://p4.com", "P4");
    checkEquals("four pages before branching", pageCount(history), 4);

    checkEquals("step back to the middle", history.back(2), "https://p2.com");
    history.visit("https://branch.com", "Branch");
    checkEquals("the branch is current", history.current_page(), "https://branch.com");
    checkEquals("forward history was discarded", pageCount(history), 3);
    checkEquals("the discarded pages are gone", snapshot(history),
                row("P1", "https://p1.com", false) +
                row("P2", "https://p2.com", false) +
                row("Branch", "https://branch.com", true));
    checkEquals("nothing lies ahead of the branch", history.forward(5), "https://branch.com");
    checkEquals("the trunk is still behind it", history.back(99), "https://p1.com");

    history.visit("https://branch2.com", "Branch2");
    checkEquals("branching from the head drops everything after it", snapshot(history),
                row("P1", "https://p1.com", false) +
                row("Branch2", "https://branch2.com", true));
    checkEquals("two pages remain", pageCount(history), 2);
}

void testZeroAndNegativeSteps() {
    section("zero and negative steps");
    BrowserHistory history("https://a.com", "A");
    history.visit("https://b.com", "B");
    history.back(1);

    checkEquals("back(0) stays put", history.back(0), "https://a.com");
    checkEquals("forward(0) stays put", history.forward(0), "https://a.com");
    checkEquals("back(-5) stays put", history.back(-5), "https://a.com");
    checkEquals("forward(-5) stays put", history.forward(-5), "https://a.com");
    checkEquals("nothing moved", history.current_page(), "https://a.com");
}

void testSinglePageAndEmpty() {
    section("single page and empty history");
    BrowserHistory history("https://only.com", "Only");
    checkEquals("back on a single page", history.back(1), "https://only.com");
    checkEquals("forward on a single page", history.forward(1), "https://only.com");
    checkEquals("still one page", pageCount(history), 1);

    history.delete_history();
    checkEquals("no current page when empty", history.current_page(), "");
    checkEquals("no current title when empty", history.current_title(), "");
    checkEquals("back on empty returns nothing", history.back(1), "");
    checkEquals("forward on empty returns nothing", history.forward(1), "");
    checkEquals("the list prints nothing", snapshot(history), "");
    checkEquals("zero pages", pageCount(history), 0);

    history.visit("https://after-clear.com", "After");
    checkEquals("visiting after a clear works", history.current_page(), "https://after-clear.com");
    checkEquals("one page again", pageCount(history), 1);
}

void testCapacity() {
    section("capacity of 50");
    BrowserHistory exact("https://page-0.example", "Page 0");
    for (int i = 1; i <= 49; i++) {
        exact.visit("https://page-" + std::to_string(i) + ".example", "Page " + std::to_string(i));
    }
    checkEquals("exactly at capacity", pageCount(exact), 50);
    checkEquals("nothing pruned yet", exact.back(1000000), "https://page-0.example");

    BrowserHistory over("https://page-0.example", "Page 0");
    for (int i = 1; i <= 60; i++) {
        over.visit("https://page-" + std::to_string(i) + ".example", "Page " + std::to_string(i));
    }
    checkEquals("the list stops growing at capacity", pageCount(over), 50);
    checkEquals("the newest page is kept", over.current_page(), "https://page-60.example");
    checkEquals("the oldest pages were pruned", over.back(1000000), "https://page-11.example");
}

void testDeleteUrl() {
    section("delete_url");
    BrowserHistory history("https://p1.com", "P1");
    history.visit("https://p2.com", "P2");
    history.visit("https://p3.com", "P3");
    history.visit("https://p4.com", "P4");

    history.delete_url("https://p2.com");
    checkEquals("deleting from the middle relinks the list", snapshot(history),
                row("P1", "https://p1.com", false) +
                row("P3", "https://p3.com", false) +
                row("P4", "https://p4.com", true));
    checkEquals("current is untouched", history.current_page(), "https://p4.com");

    history.delete_url("https://p1.com");
    checkEquals("deleting the head moves it along", snapshot(history),
                row("P3", "https://p3.com", false) +
                row("P4", "https://p4.com", true));

    history.delete_url("https://nowhere.com");
    checkEquals("deleting an unknown url changes nothing", snapshot(history),
                row("P3", "https://p3.com", false) +
                row("P4", "https://p4.com", true));

    history.delete_url("https://p4.com");
    checkEquals("deleting the current page falls back to the previous one",
                history.current_page(), "https://p3.com");
    checkEquals("one page left", pageCount(history), 1);

    history.delete_url("https://p3.com");
    checkEquals("deleting the last page empties the history", pageCount(history), 0);
    checkEquals("nothing is current", history.current_page(), "");
}

void testRestore() {
    section("restore");
    BrowserHistory history("https://p1.com", "P1");
    history.visit("https://p2.com", "P2");
    history.visit("https://p3.com", "P3");

    history.delete_url("https://p2.com");
    history.restore();
    checkEquals("restore reinserts at the original position", snapshot(history),
                row("P1", "https://p1.com", false) +
                row("P2", "https://p2.com", false) +
                row("P3", "https://p3.com", true));

    history.delete_url("https://p1.com");
    history.delete_url("https://p3.com");
    checkEquals("current follows the deleted page", history.current_page(), "https://p2.com");
    history.restore();
    checkEquals("the stack is last in first out", snapshot(history),
                row("P2", "https://p2.com", false) +
                row("P3", "https://p3.com", true));
    history.restore();
    checkEquals("the second restore brings back the head", snapshot(history),
                row("P1", "https://p1.com", false) +
                row("P2", "https://p2.com", false) +
                row("P3", "https://p3.com", true));

    std::string before = snapshot(history);
    history.restore();
    checkEquals("restoring with an empty stack changes nothing", snapshot(history), before);

    BrowserHistory single("https://only.com", "Only");
    single.delete_url("https://only.com");
    checkEquals("the history is empty after deleting its only page", pageCount(single), 0);
    single.restore();
    checkEquals("restoring into an empty history works", single.current_page(),
                "https://only.com");
    checkEquals("one page restored", pageCount(single), 1);

    BrowserHistory cleared("https://p1.com", "P1");
    cleared.visit("https://p2.com", "P2");
    cleared.delete_url("https://p1.com");
    cleared.delete_history();
    cleared.restore();
    checkEquals("delete_history clears the undo stack too", pageCount(cleared), 0);
}

void testExport() {
    section("export");
    BrowserHistory history("https://p1.com", "P1");
    history.visit("https://p2.com", "P2");
    history.visit("https://p3.com");
    history.back(1);

    check("export succeeds", history.export_history("test_tmp_export.json"));
    std::string text = readFile("test_tmp_export.json");
    check("the file records the version", contains(text, "\"version\": 1"));
    check("the file records the size", contains(text, "\"size\": 3"));
    check("the file records the current index", contains(text, "\"current_index\": 1"));
    check("the file records the capacity", contains(text, "\"capacity\": 50"));
    check("the file records a url", contains(text, "\"url\": \"https://p2.com\""));
    check("the file records a title", contains(text, "\"title\": \"P2\""));
    check("an absent title is written as empty", contains(text, "\"title\": \"\""));
    check("export to a missing folder fails", !history.export_history("no_such_folder/x.json"));

    BrowserHistory empty("https://p1.com", "P1");
    empty.delete_history();
    check("exporting an empty history succeeds", empty.export_history("test_tmp_empty.json"));
    std::string emptyText = readFile("test_tmp_empty.json");
    check("the empty file has no entries", contains(emptyText, "\"entries\": []"));
    check("the empty file has size zero", contains(emptyText, "\"size\": 0"));
    check("the empty file has no current index", contains(emptyText, "\"current_index\": -1"));
}

void testImportRoundTrip() {
    section("import round trip");
    BrowserHistory original("https://p1.com", "P1");
    original.visit("https://p2.com", "P2");
    original.visit("https://p3.com");
    original.back(1);
    original.export_history("test_tmp_roundtrip.json");

    BrowserHistory loaded("https://placeholder.com", "Placeholder");
    check("import succeeds", loaded.import_history("test_tmp_roundtrip.json"));
    checkEquals("the list survives exactly", snapshot(loaded), snapshot(original));
    checkEquals("the current page survives", loaded.current_page(), original.current_page());
    checkEquals("the current title survives", loaded.current_title(), original.current_title());
    checkEquals("three pages", pageCount(loaded), 3);
    checkEquals("navigation works on the rebuilt list", loaded.back(99), "https://p1.com");
    checkEquals("forward works on the rebuilt list", loaded.forward(99), "https://p3.com");

    BrowserHistory emptied("https://p1.com", "P1");
    emptied.delete_history();
    emptied.export_history("test_tmp_empty_rt.json");
    BrowserHistory reloaded("https://placeholder.com", "Placeholder");
    check("importing an empty history succeeds", reloaded.import_history("test_tmp_empty_rt.json"));
    checkEquals("the reloaded history is empty", pageCount(reloaded), 0);
    checkEquals("nothing is current", reloaded.current_page(), "");
}

void testImportReplacesState() {
    section("import replaces the current state");
    BrowserHistory source("https://p1.com", "P1");
    source.visit("https://p2.com", "P2");
    source.export_history("test_tmp_replace.json");

    BrowserHistory target("https://old1.com", "Old1");
    target.visit("https://old2.com", "Old2");
    target.visit("https://old3.com", "Old3");
    target.delete_url("https://old1.com");

    check("import succeeds", target.import_history("test_tmp_replace.json"));
    checkEquals("the old pages are gone", snapshot(target),
                row("P1", "https://p1.com", false) +
                row("P2", "https://p2.com", true));
    checkEquals("the page count matches the file", pageCount(target), 2);

    target.restore();
    checkEquals("import cleared the undo stack", snapshot(target),
                row("P1", "https://p1.com", false) +
                row("P2", "https://p2.com", true));
}

void testImportFailures() {
    section("import failures leave the history intact");
    BrowserHistory history("https://keep1.com", "Keep1");
    history.visit("https://keep2.com", "Keep2");
    std::string before = snapshot(history);

    check("a missing file fails", !history.import_history("test_tmp_does_not_exist.json"));
    checkEquals("history intact after a missing file", snapshot(history), before);

    writeFile("test_tmp_garbage.json", "{ this is not valid json ][");
    check("garbage fails", !history.import_history("test_tmp_garbage.json"));
    checkEquals("history intact after garbage", snapshot(history), before);

    writeFile("test_tmp_array.json", "[{\"url\":\"https://a.com\"}]");
    check("a top level array fails", !history.import_history("test_tmp_array.json"));
    checkEquals("history intact after a bare array", snapshot(history), before);

    writeFile("test_tmp_version.json",
              "{\"version\":7,\"entries\":[{\"url\":\"https://a.com\"}]}");
    check("an unsupported version fails", !history.import_history("test_tmp_version.json"));
    checkEquals("history intact after a bad version", snapshot(history), before);

    writeFile("test_tmp_noentries.json", "{\"version\":1,\"size\":2}");
    check("a missing entries array fails", !history.import_history("test_tmp_noentries.json"));
    checkEquals("history intact after a missing entries array", snapshot(history), before);

    writeFile("test_tmp_entriesobject.json", "{\"version\":1,\"entries\":{\"url\":\"https://a.com\"}}");
    check("entries that is not an array fails",
          !history.import_history("test_tmp_entriesobject.json"));
    checkEquals("history intact after a non array entries", snapshot(history), before);
}

void testImportTolerance() {
    section("import of unusual but valid files");
    BrowserHistory history("https://placeholder.com", "Placeholder");

    writeFile("test_tmp_notitle.json", "{\"version\":1,\"entries\":[{\"url\":\"https://a.com\"}]}");
    check("an entry without a title imports", history.import_history("test_tmp_notitle.json"));
    checkEquals("the missing title becomes empty", history.current_title(), "");
    checkEquals("the url is kept", history.current_page(), "https://a.com");

    writeFile("test_tmp_skip.json",
              "{\"version\":1,\"entries\":["
              "{\"url\":\"https://a.com\",\"title\":\"A\"},"
              "{\"title\":\"no url at all\"},"
              "\"junk\","
              "{\"url\":42},"
              "{\"url\":\"https://b.com\",\"title\":\"B\"}]}");
    check("malformed entries are skipped", history.import_history("test_tmp_skip.json"));
    checkEquals("only the valid entries are kept", snapshot(history),
                row("A", "https://a.com", false) +
                row("B", "https://b.com", true));

    writeFile("test_tmp_badindex.json",
              "{\"version\":1,\"current_index\":99,\"entries\":["
              "{\"url\":\"https://a.com\",\"title\":\"A\"},"
              "{\"url\":\"https://b.com\",\"title\":\"B\"}]}");
    check("an out of range current_index imports", history.import_history("test_tmp_badindex.json"));
    checkEquals("it falls back to the newest page", history.current_page(), "https://b.com");

    writeFile("test_tmp_noindex.json",
              "{\"version\":1,\"entries\":["
              "{\"url\":\"https://a.com\",\"title\":\"A\"},"
              "{\"url\":\"https://b.com\",\"title\":\"B\"}]}");
    check("a file without current_index imports", history.import_history("test_tmp_noindex.json"));
    checkEquals("it also falls back to the newest page", history.current_page(), "https://b.com");

    writeFile("test_tmp_index0.json",
              "{\"version\":1,\"current_index\":0,\"entries\":["
              "{\"url\":\"https://a.com\",\"title\":\"A\"},"
              "{\"url\":\"https://b.com\",\"title\":\"B\"}]}");
    check("current_index 0 imports", history.import_history("test_tmp_index0.json"));
    checkEquals("the first page is current", history.current_page(), "https://a.com");
    checkEquals("the marker is on the first row", snapshot(history),
                row("A", "https://a.com", true) +
                row("B", "https://b.com", false));

    writeFile("test_tmp_cap.json",
              "{\"version\":1,\"capacity\":3,\"current_index\":4,\"entries\":["
              "{\"url\":\"https://1.com\",\"title\":\"One\"},"
              "{\"url\":\"https://2.com\",\"title\":\"Two\"},"
              "{\"url\":\"https://3.com\",\"title\":\"Three\"},"
              "{\"url\":\"https://4.com\",\"title\":\"Four\"},"
              "{\"url\":\"https://5.com\",\"title\":\"Five\"}]}");
    check("a file over its own capacity imports", history.import_history("test_tmp_cap.json"));
    checkEquals("only the newest pages are kept", pageCount(history), 3);
    checkEquals("the oldest were trimmed", snapshot(history),
                row("Three", "https://3.com", false) +
                row("Four", "https://4.com", false) +
                row("Five", "https://5.com", true));
    checkEquals("the current index was shifted with the trim", history.current_page(),
                "https://5.com");

    writeFile("test_tmp_emptyentries.json", "{\"version\":1,\"entries\":[]}");
    check("an empty entries array imports", history.import_history("test_tmp_emptyentries.json"));
    checkEquals("the history is empty", pageCount(history), 0);
    checkEquals("nothing is current", history.current_page(), "");

    writeFile("test_tmp_extrakeys.json",
              "{\"version\":1,\"nonsense\":true,\"extra\":{\"a\":1},\"entries\":["
              "{\"url\":\"https://a.com\",\"title\":\"A\",\"timestamp\":\"2026-01-01\"}]}");
    check("unknown keys are ignored", history.import_history("test_tmp_extrakeys.json"));
    checkEquals("the known fields still load", snapshot(history),
                row("A", "https://a.com", true));

    writeFile("test_tmp_novers.json", "{\"entries\":[{\"url\":\"https://a.com\",\"title\":\"A\"}]}");
    check("a file without a version imports", history.import_history("test_tmp_novers.json"));
    checkEquals("one page loaded", pageCount(history), 1);
}

void testNavigationAfterImport() {
    section("branching after an import");
    BrowserHistory source("https://p1.com", "P1");
    source.visit("https://p2.com", "P2");
    source.visit("https://p3.com", "P3");
    source.export_history("test_tmp_branch.json");

    BrowserHistory loaded("https://placeholder.com", "Placeholder");
    loaded.import_history("test_tmp_branch.json");
    loaded.back(1);
    loaded.visit("https://branch.com", "Branch");
    checkEquals("branching works on an imported list", snapshot(loaded),
                row("P1", "https://p1.com", false) +
                row("P2", "https://p2.com", false) +
                row("Branch", "https://branch.com", true));

    loaded.delete_url("https://p2.com");
    loaded.restore();
    checkEquals("delete and restore work on an imported list", snapshot(loaded),
                row("P1", "https://p1.com", false) +
                row("P2", "https://p2.com", false) +
                row("Branch", "https://branch.com", true));
}

void cleanUp() {
    const char* files[] = {
        "test_tmp_export.json", "test_tmp_empty.json", "test_tmp_roundtrip.json",
        "test_tmp_empty_rt.json", "test_tmp_replace.json", "test_tmp_garbage.json",
        "test_tmp_array.json", "test_tmp_version.json", "test_tmp_noentries.json",
        "test_tmp_entriesobject.json", "test_tmp_notitle.json", "test_tmp_skip.json",
        "test_tmp_badindex.json", "test_tmp_noindex.json", "test_tmp_index0.json",
        "test_tmp_cap.json", "test_tmp_emptyentries.json", "test_tmp_extrakeys.json",
        "test_tmp_novers.json", "test_tmp_branch.json"
    };
    for (const char* file : files) {
        std::remove(file);
    }
}

}

int main() {
    std::cout << "================================================================" << std::endl;
    std::cout << " BROWSER HISTORY TEST SUITE" << std::endl;
    std::cout << "================================================================" << std::endl;

    testNode();
    testConstruction();
    testVisit();
    testBackOneHundredTimesOnTwoPages();
    testBranchingFromTheMiddle();
    testZeroAndNegativeSteps();
    testSinglePageAndEmpty();
    testCapacity();
    testDeleteUrl();
    testRestore();
    testExport();
    testImportRoundTrip();
    testImportReplacesState();
    testImportFailures();
    testImportTolerance();
    testNavigationAfterImport();

    cleanUp();

    std::cout << "\n================================================================" << std::endl;
    std::cout << " " << (checksRun - checksFailed) << " of " << checksRun << " checks passed"
              << std::endl;
    if (checksFailed == 0) {
        std::cout << " ALL CHECKS PASSED" << std::endl;
    } else {
        std::cout << " " << checksFailed << " CHECK(S) FAILED" << std::endl;
    }
    std::cout << "================================================================" << std::endl;

    return checksFailed == 0 ? 0 : 1;
}
