#include "../v0/dictionary.hh"
#include "../v1/dictionary.hh"
#include "../v2/dictionary.hh"
#include "../v3/dictionary.hh"
#include "../v4/dictionary.hh"

#include <benchmark/benchmark.h>

// instrumentation
#include <papipp.h>
#include <valgrind/callgrind.h>


#include <iostream>
#include <vector>
#include <random>
#include <unordered_set>


static std::random_device rd;
static std::uniform_int_distribution<int> charDist(48, 127);
static std::lognormal_distribution<float> lengthDist(2.19, 0.25);

std::vector<std::string> wordsIn;
std::vector<std::string> wordsNotIn;

static std::string CreateRandomString()
{
	std::size_t len = (int)(std::floor(lengthDist(rd))) + 1;
	std::string out;
	out.reserve(len);
	for(std::size_t i = 0; i < len; i++)
		out.push_back(charDist(rd));

	return out;
}

static std::vector<std::string> CreateVectorOfUniqueRandomStrings(std::size_t len)
{
	std::unordered_set<std::string> strings;
	strings.reserve(len);

	while (strings.size() < len)
		strings.insert(CreateRandomString());

	return std::vector<std::string>(strings.begin(), strings.end());
}

template<typename DictType>
void InDictionary(benchmark::State& state)
{
	int idx = 0;
	DictType dict(wordsIn );
	bool allFound = true;

	// papi - perf counters
	//papi::event_set<PAPI_TOT_INS, PAPI_TOT_CYC, PAPI_BR_MSP, PAPI_L1_DCM> events;
	//events.start_counters();

	// enable Callgrind instrumentation
	//CALLGRIND_START_INSTRUMENTATION;

	for(auto _ : state)
	{
		const std::string& word = wordsIn[idx];
		idx = (idx+1) % wordsIn.size();
		benchmark::DoNotOptimize(allFound &= dict.in_dictionary(word));
	}

	//CALLGRIND_STOP_INSTRUMENTATION; // vcallgrind

	//events.stop_counters();
	//std::cout << events.get<PAPI_TOT_INS>().counter()/double(events.get<PAPI_TOT_CYC>().counter()) << " instr per cycle\n";
	//std::cout << events.get<PAPI_TOT_INS>().counter()/double(state.iterations()) << " instructions\n";
	//std::cout << events.get<PAPI_L1_DCM>().counter()/double(state.iterations()) << " l1 cache misses\n"
	//		  << events.get<PAPI_BR_MSP>().counter()/double(state.iterations()) << " branch misses" << std::endl;
	//std::cout << "iterations: " << state.iterations() << std::endl;

	if (!allFound)
		std::cout << "InDictionary consistency check failed" << std::endl;
}

template<typename DictType>
void NotInDictionary(benchmark::State& state)
{
	int idx = 0;
	DictType dict(wordsIn);
	bool someFound = false;

	for(auto _ : state)
	{
		const std::string& word = wordsNotIn[idx];
		idx = (idx+1) % wordsNotIn.size();
		benchmark::DoNotOptimize(someFound |= dict.in_dictionary(word));
	}

	if (someFound)
		std::cout << "NotInDictionary consistency check failed" << std::endl;
}

BENCHMARK_TEMPLATE(InDictionary, v0::Dictionary);
BENCHMARK_TEMPLATE(NotInDictionary, v0::Dictionary);

BENCHMARK_TEMPLATE(InDictionary, v1::Dictionary);
BENCHMARK_TEMPLATE(NotInDictionary, v1::Dictionary);

BENCHMARK_TEMPLATE(InDictionary, v2::Dictionary);
BENCHMARK_TEMPLATE(NotInDictionary, v2::Dictionary);

BENCHMARK_TEMPLATE(InDictionary, v3::Dictionary);
BENCHMARK_TEMPLATE(NotInDictionary, v3::Dictionary);

BENCHMARK_TEMPLATE(InDictionary, v4::Dictionary);
BENCHMARK_TEMPLATE(NotInDictionary, v4::Dictionary);


int main(int argc, char** argv)
{
	static const int DICT_SIZE = 99171;
	std::vector<std::string> words = CreateVectorOfUniqueRandomStrings(DICT_SIZE*2);
	wordsIn.assign(words.begin(), words.begin() + DICT_SIZE);
	wordsNotIn.assign(words.begin() + DICT_SIZE, words.end());


	benchmark::Initialize(&argc, argv);
	if (::benchmark::ReportUnrecognizedArguments(argc, argv))
		return 1;
	benchmark::RunSpecifiedBenchmarks();
}
