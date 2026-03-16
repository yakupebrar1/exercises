// runoff.c
// CS50x — Week 3, Problem Set 3
//
// Exercise: Instant Runoff Voting
// ─────────────────────────────────
// Voters rank ALL candidates in order of preference (1st, 2nd, 3rd, …).
// If no candidate earns a strict majority (> 50% of votes), the candidate(s)
// with the fewest votes are eliminated and their ballots are redistributed to
// voters' next valid choice. Repeat until a winner emerges or a tie is declared.
//
// Usage:   ./runoff Alice Bob Charlie
// Check:   check50 cs50/problems/2024/x/runoff
// Style:   style50 runoff.c

#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

// ── Limits ────────────────────────────────────────────────────────────────────
#define MAX_VOTERS     100
#define MAX_CANDIDATES   9

// ── Data ──────────────────────────────────────────────────────────────────────
// preferences[i][j] = the index (in candidates[]) of voter i's j-th ranked choice
int preferences[MAX_VOTERS][MAX_CANDIDATES];

// Each candidate has a name, current round vote total, and an elimination flag
typedef struct
{
    string name;
    int    votes;
    bool   eliminated;
} candidate;

candidate candidates[MAX_CANDIDATES];

// Global counts set in main()
int voter_count;
int candidate_count;

// ── Function prototypes — do NOT change these signatures ──────────────────────
bool vote(int voter, int rank, string name);
void tabulate(void);
bool print_winner(void);
int  find_min(void);
bool is_tie(int min);
void eliminate(int min);

// ── main() — provided, do not modify ─────────────────────────────────────────
int main(int argc, string argv[])
{
    if (argc < 2)
    {
        printf("Usage: runoff [candidate ...]\n");
        return 1;
    }

    candidate_count = argc - 1;
    if (candidate_count > MAX_CANDIDATES)
    {
        printf("Maximum number of candidates is %i\n", MAX_CANDIDATES);
        return 2;
    }

    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name       = argv[i + 1];
        candidates[i].votes      = 0;
        candidates[i].eliminated = false;
    }

    voter_count = get_int("Number of voters: ");
    if (voter_count > MAX_VOTERS)
    {
        printf("Maximum number of voters is %i\n", MAX_VOTERS);
        return 3;
    }

    // Collect each voter's full ranking
    for (int i = 0; i < voter_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);
            if (!vote(i, j, name))
            {
                printf("Invalid vote.\n");
                return 4;
            }
        }
        printf("\n");
    }

    // Run rounds until we have a winner or a tie
    while (true)
    {
        tabulate();

        bool won = print_winner();
        if (won)
        {
            break;
        }

        int  min = find_min();
        bool tie = is_tie(min);

        if (tie)
        {
            // All remaining candidates tie — print them all
            for (int i = 0; i < candidate_count; i++)
            {
                if (!candidates[i].eliminated)
                {
                    printf("%s\n", candidates[i].name);
                }
            }
            break;
        }

        eliminate(min);

        // Reset vote counts for next round
        for (int i = 0; i < candidate_count; i++)
        {
            candidates[i].votes = 0;
        }
    }
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// TODO 1: vote(voter, rank, name)
// ─────────────────────────────────────────────────────────────────────────────
//
// Record that `voter`'s `rank`-th preference is the candidate named `name`.
//
// How:
//   • Search candidates[] for a candidate whose name matches `name`
//     (use strcasecmp for case-insensitive comparison).
//   • If found: store the candidate's INDEX in preferences[voter][rank]
//               and return true.
//   • If not found: return false.
//
// Example:
//   Candidates: Alice=0, Bob=1, Charlie=2
//   vote(0, 0, "Bob")  → preferences[0][0] = 1, return true
//   vote(0, 1, "alice")→ preferences[0][1] = 0, return true
//   vote(0, 2, "Dave") → return false
// ─────────────────────────────────────────────────────────────────────────────
bool vote(int voter, int rank, string name)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcasecmp(candidates[i].name, name) == 0)
        {
            preferences[voter][rank] = i;
            return true;
        }
    }

    return false;
}
// ─────────────────────────────────────────────────────────────────────────────
// TODO 2: tabulate()
// ─────────────────────────────────────────────────────────────────────────────
//
// Update vote counts based on the current round.
//
// For each voter, find their HIGHEST-RANKED candidate who has NOT been
// eliminated, and increment that candidate's votes by 1.
//
// How:
//   • Outer loop: iterate over voters (0 to voter_count)
//   • Inner loop: iterate over ranks (0 to candidate_count)
//       – Look up the candidate index: int c = preferences[voter][rank];
//       – If candidates[c].eliminated == false:
//             candidates[c].votes++;
//             break  (only count ONE vote per voter per round)
//
// Note: Reset vote counts back to 0 happen in main() before calling tabulate()
//       each round — you don't need to reset them here.
// ─────────────────────────────────────────────────────────────────────────────
void tabulate(void)
{
    for (int i = 0; i < voter_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            int c = preferences[i][j];

            if (!candidates[c].eliminated)
            {
                candidates[c].votes++;
                break;
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// TODO 3: print_winner()
// ─────────────────────────────────────────────────────────────────────────────
//
// If any non-eliminated candidate has STRICTLY MORE than half the total votes,
// print their name and return true. Otherwise return false.
//
// Majority threshold: candidates[i].votes > voter_count / 2
//
// Example: 5 voters → majority threshold = > 2.5 → needs at least 3 votes.
// ─────────────────────────────────────────────────────────────────────────────
bool print_winner(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (!candidates[i].eliminated &&
            candidates[i].votes > voter_count / 2)
        {
            printf("%s\n", candidates[i].name);
            return true;
        }
    }

    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// TODO 4: find_min()
// ─────────────────────────────────────────────────────────────────────────────
//
// Return the MINIMUM vote total among all non-eliminated candidates.
//
// How:
//   • Start min at a large value (e.g., voter_count + 1 or INT_MAX).
//   • Loop through candidates[]; skip any with eliminated == true.
//   • If candidates[i].votes < min, update min.
//   • Return min.
// ─────────────────────────────────────────────────────────────────────────────
int find_min(void)
{
    int min = voter_count;

    for (int i = 0; i < candidate_count; i++)
    {
        if (!candidates[i].eliminated &&
            candidates[i].votes < min)
        {
            min = candidates[i].votes;
        }
    }

    return min;
}

// ─────────────────────────────────────────────────────────────────────────────
// TODO 5: is_tie(min)
// ─────────────────────────────────────────────────────────────────────────────
//
// Return true if EVERY non-eliminated candidate has exactly `min` votes.
// This means all remaining candidates are perfectly tied — declare them
// all winners (handled in main).
//
// How:
//   • Loop through candidates[]; skip eliminated ones.
//   • If any non-eliminated candidate has votes != min, return false.
//   • If we finish the loop without finding such a candidate, return true.
// ─────────────────────────────────────────────────────────────────────────────
bool is_tie(int min)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (!candidates[i].eliminated &&
            candidates[i].votes != min)
        {
            return false;
        }
    }

    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// TODO 6: eliminate(min)
// ─────────────────────────────────────────────────────────────────────────────
//
// Eliminate all non-eliminated candidates who have exactly `min` votes.
// Mark them by setting candidates[i].eliminated = true.
//
// Note: Do NOT change candidates who are already eliminated.
// ─────────────────────────────────────────────────────────────────────────────
void eliminate(int min)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (!candidates[i].eliminated &&
            candidates[i].votes == min)
        {
            candidates[i].eliminated = true;
        }
    }
}
