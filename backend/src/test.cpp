#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "ds_utility.h"
#include "linked_list.h"
#include "bst.h"
#include "min_heap.h"
#include "hash_table.h"
#include "calendar.h"
#include "file_manager.h"

// ============================================================
// Event::getEndTime
// ============================================================
TEST_CASE("Event::getEndTime") {
    Event e;

    SUBCASE("basic duration") {
        e.startTime = "09:00";
        e.durationMins = 60;
        CHECK(e.getEndTime() == "10:00");
    }

    SUBCASE("overflow to next hour") {
        e.startTime = "09:45";
        e.durationMins = 30;
        CHECK(e.getEndTime() == "10:15");
    }

    SUBCASE("overflow to next day") {
        e.startTime = "23:30";
        e.durationMins = 45;
        CHECK(e.getEndTime() == "00:15");
    }

    SUBCASE("exactly 24 hours") {
        e.startTime = "00:00";
        e.durationMins = 1440;
        CHECK(e.getEndTime() == "00:00");
    }

    SUBCASE("empty start time") {
        e.startTime = "";
        e.durationMins = 30;
        CHECK(e.getEndTime() == "00:00");
    }

    SUBCASE("zero duration") {
        e.startTime = "14:30";
        e.durationMins = 0;
        CHECK(e.getEndTime() == "14:30");
    }
}

// ============================================================
// ds_utility
// ============================================================
TEST_CASE("ds_utility::timeToMinutes") {
    CHECK(timeToMinutes("00:00") == 0);
    CHECK(timeToMinutes("01:00") == 60);
    CHECK(timeToMinutes("12:30") == 750);
    CHECK(timeToMinutes("23:59") == 1439);
    CHECK(timeToMinutes("") == 0);
    CHECK(timeToMinutes("invalid") == 0);
}

TEST_CASE("ds_utility::convertToComparable") {
    CHECK(convertToComparable("15:03:2026") == "20260315");
    CHECK(convertToComparable("01:01:2020") == "20200101");
    CHECK(convertToComparable("31:12:1999") == "19991231");
    CHECK(convertToComparable("invalid") == "invalid");
    CHECK(convertToComparable("") == "");
}

TEST_CASE("ds_utility::escapeCSV") {
    CHECK(escapeCSV("hello") == "hello");
    CHECK(escapeCSV("hello,world") == "\"hello,world\"");
    CHECK(escapeCSV("say \"hi\"") == "\"say \"\"hi\"\"\"");
    CHECK(escapeCSV("line1\nline2") == "\"line1\nline2\"");
    CHECK(escapeCSV("") == "");
}

TEST_CASE("ds_utility::parseCSVLine") {
    auto fields = parseCSVLine("a,b,c");
    CHECK(fields.size() == 3);
    CHECK(fields[0] == "a");
    CHECK(fields[1] == "b");
    CHECK(fields[2] == "c");

    fields = parseCSVLine("\"hello,world\",42");
    CHECK(fields.size() == 2);
    CHECK(fields[0] == "hello,world");
    CHECK(fields[1] == "42");

    fields = parseCSVLine("\"say \"\"hi\"\"\",end");
    CHECK(fields.size() == 2);
    CHECK(fields[0] == "say \"hi\"");
    CHECK(fields[1] == "end");

    fields = parseCSVLine("single");
    CHECK(fields.size() == 1);
    CHECK(fields[0] == "single");

    fields = parseCSVLine("");
    CHECK(fields.size() == 1);
    CHECK(fields[0] == "");
}

// ============================================================
// LinkedList
// ============================================================
TEST_CASE("LinkedList") {
    LinkedList list;

    SUBCASE("empty list") {
        CHECK(list.count() == 0);
        CHECK(list.getHead() == nullptr);
    }

    SUBCASE("insertSorted and count") {
        Event e1, e2, e3;
        e1.startTime = "10:00"; e1.id = "EVT_1";
        e2.startTime = "08:00"; e2.id = "EVT_2";
        e3.startTime = "09:00"; e3.id = "EVT_3";

        list.insertSorted(e1);
        list.insertSorted(e2);
        list.insertSorted(e3);

        CHECK(list.count() == 3);

        ListNode* curr = list.getHead();
        CHECK(curr->data.id == "EVT_2");
        curr = curr->next.get();
        CHECK(curr->data.id == "EVT_3");
        curr = curr->next.get();
        CHECK(curr->data.id == "EVT_1");
        CHECK(curr->next.get() == nullptr);
    }

    SUBCASE("append") {
        Event e1, e2;
        e1.startTime = "10:00"; e1.id = "EVT_1";
        e2.startTime = "08:00"; e2.id = "EVT_2";

        list.append(e1);
        list.append(e2);

        CHECK(list.count() == 2);
        ListNode* curr = list.getHead();
        CHECK(curr->data.id == "EVT_1");
        curr = curr->next.get();
        CHECK(curr->data.id == "EVT_2");
    }

    SUBCASE("removeByID") {
        Event e1, e2, e3;
        e1.id = "EVT_1"; e1.startTime = "10:00";
        e2.id = "EVT_2"; e2.startTime = "08:00";
        e3.id = "EVT_3"; e3.startTime = "09:00";

        list.insertSorted(e1);
        list.insertSorted(e2);
        list.insertSorted(e3);

        CHECK(list.removeByID("EVT_2") == true);
        CHECK(list.count() == 2);
        CHECK(list.getHead()->data.id == "EVT_3");

        CHECK(list.removeByID("EVT_1") == true);
        CHECK(list.count() == 1);
        CHECK(list.getHead()->data.id == "EVT_3");

        CHECK(list.removeByID("EVT_3") == true);
        CHECK(list.count() == 0);
        CHECK(list.getHead() == nullptr);

        CHECK(list.removeByID("NONEXISTENT") == false);
    }

    SUBCASE("checkConflict") {
        Event e1, e2, e3;
        e1.startTime = "09:00"; e1.durationMins = 60; e1.id = "EVT_1";
        e2.startTime = "10:00"; e2.durationMins = 60; e2.id = "EVT_2";
        e3.startTime = "10:30"; e3.durationMins = 30; e3.id = "EVT_3";

        list.insertSorted(e1);  // 09:00-10:00
        list.insertSorted(e2);  // 10:00-11:00 (adjacent, no overlap)
        list.insertSorted(e3);  // 10:30-11:00 (overlaps with e2)

        Event* conflict = list.checkConflict("10:00", "11:00");
        CHECK(conflict != nullptr);

        conflict = list.checkConflict("08:00", "09:00");
        CHECK(conflict == nullptr);

        conflict = list.checkConflict("11:00", "12:00");
        CHECK(conflict == nullptr);
    }

    SUBCASE("clear") {
        Event e1, e2;
        e1.id = "EVT_1"; e2.id = "EVT_2";
        list.insertSorted(e1);
        list.insertSorted(e2);
        CHECK(list.count() == 2);
        list.clear();
        CHECK(list.count() == 0);
        CHECK(list.getHead() == nullptr);
    }

    SUBCASE("copy constructor") {
        Event e1, e2;
        e1.startTime = "10:00"; e1.id = "EVT_1";
        e2.startTime = "08:00"; e2.id = "EVT_2";
        list.insertSorted(e1);
        list.insertSorted(e2);

        LinkedList copy(list);
        CHECK(copy.count() == 2);
        CHECK(copy.getHead()->data.id == "EVT_2");

        copy.removeByID("EVT_2");
        CHECK(copy.count() == 1);
        CHECK(list.count() == 2);
    }

    SUBCASE("assignment operator") {
        Event e1, e2;
        e1.startTime = "10:00"; e1.id = "EVT_1";
        e2.startTime = "08:00"; e2.id = "EVT_2";
        list.insertSorted(e1);
        list.insertSorted(e2);

        LinkedList other;
        other = list;
        CHECK(other.count() == 2);
        CHECK(other.getHead()->data.id == "EVT_2");
    }
}

// ============================================================
// BST
// ============================================================
TEST_CASE("BST") {
    BST tree;

    SUBCASE("empty tree") {
        CHECK(tree.isEmpty() == true);
        CHECK(tree.search("15:03:2026") == nullptr);
    }

    SUBCASE("insert and search") {
        Event e1, e2;
        e1.id = "EVT_1"; e1.startTime = "10:00";
        e2.id = "EVT_2"; e2.startTime = "14:00";

        tree.insert("15:03:2026", e1);
        CHECK(tree.isEmpty() == false);

        BSTNode* node = tree.search("15:03:2026");
        REQUIRE(node != nullptr);
        CHECK(node->date == "15:03:2026");
        CHECK(node->events.getHead()->data.id == "EVT_1");
        CHECK(node->events.count() == 1);
    }

    SUBCASE("multiple events on same date") {
        Event e1, e2;
        e1.id = "EVT_1"; e1.startTime = "10:00";
        e2.id = "EVT_2"; e2.startTime = "14:00";

        tree.insert("15:03:2026", e1);
        tree.insert("15:03:2026", e2);

        BSTNode* node = tree.search("15:03:2026");
        REQUIRE(node != nullptr);
        CHECK(node->events.count() == 2);
    }

    SUBCASE("multiple dates") {
        Event e1, e2, e3;
        e1.id = "EVT_1"; e1.startTime = "10:00";
        e2.id = "EVT_2"; e2.startTime = "10:00";
        e3.id = "EVT_3"; e3.startTime = "10:00";

        tree.insert("15:03:2026", e1);
        tree.insert("10:01:2026", e2);
        tree.insert("20:12:2025", e3);

        CHECK(tree.search("15:03:2026") != nullptr);
        CHECK(tree.search("10:01:2026") != nullptr);
        CHECK(tree.search("20:12:2025") != nullptr);
        CHECK(tree.search("01:01:2099") == nullptr);
    }

    SUBCASE("forEach") {
        Event e1, e2;
        e1.id = "EVT_A"; e1.startTime = "10:00";
        e2.id = "EVT_B"; e2.startTime = "14:00";

        tree.insert("15:03:2026", e1);
        tree.insert("10:01:2026", e2);

        int count = 0;
        tree.forEach([&](const Event& e) { count++; });
        CHECK(count == 2);
    }

    SUBCASE("copy constructor") {
        Event e1, e2;
        e1.id = "EVT_1"; e1.startTime = "10:00";
        e2.id = "EVT_2"; e2.startTime = "14:00";

        tree.insert("15:03:2026", e1);
        tree.insert("10:01:2026", e2);

        BST copy(tree);
        CHECK_FALSE(copy.isEmpty());
        CHECK(copy.search("15:03:2026") != nullptr);
        CHECK(copy.search("10:01:2026") != nullptr);

        copy.insert("20:12:2025", e1);
        CHECK(copy.search("20:12:2025") != nullptr);
        CHECK(tree.search("20:12:2025") == nullptr);
    }
}

// ============================================================
// MinHeap
// ============================================================
TEST_CASE("MinHeap") {
    MinHeap heap;

    SUBCASE("empty heap") {
        CHECK(heap.isEmpty());
        CHECK(heap.getSize() == 0);
    }

    SUBCASE("insert and peek") {
        Event e1, e2, e3;
        e1.id = "EVT_1"; e1.date = "15:03:2026"; e1.startTime = "14:00";
        e2.id = "EVT_2"; e2.date = "10:01:2026"; e2.startTime = "09:00";
        e3.id = "EVT_3"; e3.date = "20:12:2025"; e3.startTime = "10:00";

        heap.insert(e1);
        heap.insert(e2);
        heap.insert(e3);

        CHECK(heap.getSize() == 3);
        CHECK(heap.peek().id == "EVT_3");
    }

    SUBCASE("extractMin in order") {
        Event e1, e2, e3;
        e1.id = "EVT_1"; e1.date = "15:03:2026"; e1.startTime = "14:00";
        e2.id = "EVT_2"; e2.date = "10:01:2026"; e2.startTime = "09:00";
        e3.id = "EVT_3"; e3.date = "20:12:2025"; e3.startTime = "10:00";

        heap.insert(e1);
        heap.insert(e2);
        heap.insert(e3);

        CHECK(heap.extractMin().id == "EVT_3");
        CHECK(heap.extractMin().id == "EVT_2");
        CHECK(heap.extractMin().id == "EVT_1");
        CHECK(heap.isEmpty());
    }

    SUBCASE("same date, different times") {
        Event e1, e2;
        e1.id = "EVT_1"; e1.date = "15:03:2026"; e1.startTime = "14:00";
        e2.id = "EVT_2"; e2.date = "15:03:2026"; e2.startTime = "09:00";

        heap.insert(e1);
        heap.insert(e2);

        CHECK(heap.extractMin().id == "EVT_2");
        CHECK(heap.extractMin().id == "EVT_1");
    }

    SUBCASE("remove by ID") {
        Event e1, e2, e3;
        e1.id = "EVT_1"; e1.date = "15:03:2026"; e1.startTime = "14:00";
        e2.id = "EVT_2"; e2.date = "10:01:2026"; e2.startTime = "09:00";
        e3.id = "EVT_3"; e3.date = "20:12:2025"; e3.startTime = "10:00";

        heap.insert(e1);
        heap.insert(e2);
        heap.insert(e3);

        CHECK(heap.remove("EVT_1") == true);
        CHECK(heap.getSize() == 2);
        CHECK(heap.remove("NONEXISTENT") == false);
        CHECK(heap.getSize() == 2);
        CHECK(heap.extractMin().id == "EVT_3");
        CHECK(heap.extractMin().id == "EVT_2");
    }

    SUBCASE("clear") {
        Event e1;
        e1.id = "EVT_1"; e1.date = "15:03:2026"; e1.startTime = "14:00";
        heap.insert(e1);
        CHECK_FALSE(heap.isEmpty());
        heap.clear();
        CHECK(heap.isEmpty());
        CHECK(heap.getSize() == 0);
    }
}

// ============================================================
// HashTable
// ============================================================
TEST_CASE("HashTable") {
    HashTable ht;

    SUBCASE("empty table") {
        CHECK(ht.search("NONEXISTENT") == nullptr);
    }

    SUBCASE("insert and search") {
        Event e1, e2;
        e1.id = "EVT_001";
        e2.id = "EVT_002";

        ht.insert(e1);
        ht.insert(e2);

        Event* found = ht.search("EVT_001");
        REQUIRE(found != nullptr);
        CHECK(found->id == "EVT_001");

        found = ht.search("EVT_002");
        REQUIRE(found != nullptr);
        CHECK(found->id == "EVT_002");

        CHECK(ht.search("EVT_999") == nullptr);
    }

    SUBCASE("remove") {
        Event e1;
        e1.id = "EVT_001";
        ht.insert(e1);

        CHECK(ht.remove("EVT_001") == true);
        CHECK(ht.search("EVT_001") == nullptr);
        CHECK(ht.remove("EVT_001") == false);
    }

    SUBCASE("clear") {
        Event e1, e2;
        e1.id = "EVT_001";
        e2.id = "EVT_002";
        ht.insert(e1);
        ht.insert(e2);
        ht.clear();
        CHECK(ht.search("EVT_001") == nullptr);
        CHECK(ht.search("EVT_002") == nullptr);
    }

    SUBCASE("hash function consistency") {
        int idx1 = ht.hashFunction("EVT_001");
        int idx2 = ht.hashFunction("EVT_001");
        CHECK(idx1 == idx2);
    }
}

// ============================================================
// CalendarSystem (integration tests)
// ============================================================
TEST_CASE("CalendarSystem") {
    CalendarSystem cal;

    SUBCASE("add and retrieve event") {
        bool added = cal.addEvent("Meeting", "15:03:2026", "10:00", 60, 3, "Team sync");
        CHECK(added == true);

        auto events = cal.getEventsByDate("15:03:2026");
        CHECK(events.size() == 1);
        CHECK(events[0].title == "Meeting");
        CHECK(events[0].startTime == "10:00");
        CHECK(events[0].durationMins == 60);
        CHECK(events[0].priority == 3);
        CHECK(events[0].description == "Team sync");
        CHECK(events[0].id.substr(0, 4) == "EVT_");
    }

    SUBCASE("conflict detection") {
        cal.addEvent("Event A", "15:03:2026", "10:00", 60, 3, "");
        bool added = cal.addEvent("Event B", "15:03:2026", "10:30", 60, 3, "");
        CHECK(added == false);

        added = cal.addEvent("Event B", "15:03:2026", "11:00", 60, 3, "");
        CHECK(added == true);
    }

    SUBCASE("checkConflicts") {
        cal.addEvent("A", "15:03:2026", "09:00", 60, 3, "");
        cal.addEvent("B", "15:03:2026", "09:30", 60, 3, "");
        cal.addEvent("C", "15:03:2026", "11:00", 60, 3, "");

        ConflictResult result = cal.checkConflicts("15:03:2026");
        CHECK(result.conflicts.size() == 1);

        result = cal.checkConflicts("16:03:2026");
        CHECK(result.conflicts.size() == 0);
    }

    SUBCASE("search events") {
        cal.addEvent("Team Meeting", "15:03:2026", "10:00", 60, 3, "");
        cal.addEvent("Lunch", "15:03:2026", "12:00", 60, 3, "");
        cal.addEvent("Team Standup", "16:03:2026", "09:00", 15, 2, "");

        auto results = cal.searchEvents("Team");
        CHECK(results.size() == 2);

        results = cal.searchEvents("Lunch");
        CHECK(results.size() == 1);

        results = cal.searchEvents("NonExistent");
        CHECK(results.size() == 0);
    }

    SUBCASE("viewUpcoming") {
        cal.addEvent("Later", "20:12:2026", "10:00", 60, 3, "");
        cal.addEvent("Earlier", "10:01:2026", "10:00", 60, 3, "");

        auto upcoming = cal.viewUpcoming(10);
        CHECK(upcoming.size() == 2);
        CHECK(upcoming[0].title == "Earlier");
        CHECK(upcoming[1].title == "Later");
    }

    SUBCASE("delete event") {
        cal.addEvent("To Delete", "15:03:2026", "10:00", 60, 3, "");

        auto events = cal.getEventsByDate("15:03:2026");
        CHECK(events.size() == 1);

        bool deleted = cal.deleteEvent(events[0].id);
        CHECK(deleted == true);

        events = cal.getEventsByDate("15:03:2026");
        CHECK(events.size() == 0);

        deleted = cal.deleteEvent("NONEXISTENT");
        CHECK(deleted == false);
    }

    SUBCASE("update event") {
        cal.addEvent("Original", "15:03:2026", "10:00", 60, 3, "");

        auto events = cal.getEventsByDate("15:03:2026");
        string id = events[0].id;

        bool updated = cal.updateEvent(id, "Updated", "15:03:2026", "10:00", 90, 1, "New desc");
        CHECK(updated == true);

        Event* e = cal.getEventById(id);
        REQUIRE(e != nullptr);
        CHECK(e->title == "Updated");
        CHECK(e->durationMins == 90);
        CHECK(e->priority == 1);
        CHECK(e->description == "New desc");
    }

    SUBCASE("update event with date change") {
        cal.addEvent("Move Me", "15:03:2026", "10:00", 60, 3, "");
        cal.addEvent("Occupier", "16:03:2026", "10:00", 60, 3, "");

        auto events = cal.getEventsByDate("15:03:2026");
        string id = events[0].id;

        bool updated = cal.updateEvent(id, "Move Me", "17:03:2026", "10:00", 60, 3, "");
        CHECK(updated == true);

        auto oldDate = cal.getEventsByDate("15:03:2026");
        CHECK(oldDate.size() == 0);

        auto newDate = cal.getEventsByDate("17:03:2026");
        CHECK(newDate.size() == 1);
    }

    SUBCASE("clear all events") {
        cal.addEvent("A", "15:03:2026", "10:00", 60, 3, "");
        cal.addEvent("B", "16:03:2026", "10:00", 60, 3, "");

        cal.clearAllEvents();

        CHECK(cal.getEventsByDate("15:03:2026").size() == 0);
        CHECK(cal.getEventsByDate("16:03:2026").size() == 0);
        CHECK(cal.viewUpcoming(10).size() == 0);
    }

    SUBCASE("getEventById") {
        cal.addEvent("Findable", "15:03:2026", "10:00", 60, 3, "");

        auto events = cal.getEventsByDate("15:03:2026");
        string id = events[0].id;

        Event* found = cal.getEventById(id);
        REQUIRE(found != nullptr);
        CHECK(found->title == "Findable");

        CHECK(cal.getEventById("NONEXISTENT") == nullptr);
    }
}

// ============================================================
// File manager integration (limited, uses file I/O)
// ============================================================
TEST_CASE("FileManager") {
    CalendarSystem cal;
    cal.addEvent("SaveTest", "15:03:2026", "10:00", 60, 3, "desc");

    SUBCASE("save and load") {
        bool saved = saveEventsToFile("test_calendar_data.txt", cal);
        CHECK(saved == true);

        CalendarSystem cal2;
        int loaded = loadEventsFromFile("test_calendar_data.txt", cal2);
        CHECK(loaded == 1);

        auto events = cal2.getEventsByDate("15:03:2026");
        CHECK(events.size() == 1);
        CHECK(events[0].title == "SaveTest");
        CHECK(events[0].description == "desc");
    }

    SUBCASE("export formatted TXT") {
        bool exported = cal.exportFormattedTXT("test_export.txt");
        CHECK(exported == true);
    }
}
