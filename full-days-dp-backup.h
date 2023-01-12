using dp_key_t = uint64_t;
using dp_indices_t = tuple<int, int, int, int, int>;
using dp_value_t = uint16_t;

dp_key_t dp_key(const int r, const int b, const int w, const int p, const int d){
    constexpr int bits = 12;
    return
        ((dp_key_t)r << (bits * 4)) + 
        ((dp_key_t)b << (bits * 3)) + 
        ((dp_key_t)w << (bits * 2)) + 
        ((dp_key_t)p << bits) + 
        d;
}

dp_key_t dp_key(const dp_indices_t i){
    constexpr int bits = 12;
    return dp_key(get<0>(i), get<1>(i), get<2>(i), get<3>(i), get<4>(i));
}

dp_indices_t dp_indices(const dp_key_t key){
    constexpr int bits = 12;
    constexpr int mask = (1 << bits) - 1;
    return {
        key >> (bits * 4),
        (key >> (bits * 3)) & mask,
        (key >> (bits * 2)) & mask,
        (key >> bits) & mask,
        key & mask
    };
}

class Condition{
    public:
    int day;
    int money;
};

int full_days_dp_solve(const int k, const bool print_result = false){
    unordered_map<dp_key_t, dp_value_t> dp;
    constexpr int R = REQUIRED[(int)Color::Red];
    constexpr int B = BLUE_UPPER_LIMIT;
    constexpr int P = REQUIRED[(int)Color::Purple];
    constexpr int W = REQUIRED[(int)Color::White];
    constexpr int D = DAY_MAX;
    constexpr int EMPTY = numeric_limits<dp_value_t>::max();
    dp[dp_key(0, 0, 0, 0, 1)] = 0;
    for(int day = 1; day <= DAY_MAX; day++){
        for(int r = 0; r <= R; r++){
            for(int b = 0; b <= B; b++){
                for(int p = 0; p <= P; p++){
                    for(int w = 0; w <= W; w++){
                        //
                    }
                }
            }
        }
    }
    return sizeof(dp);
}