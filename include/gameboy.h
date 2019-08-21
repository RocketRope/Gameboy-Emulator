#ifndef _GAMEBOY_H_
#define _GAMEBOY_H_

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "el/easylogging++.h"

#include "types.h"

#include "cpu.h"
#include "video.h"
#include "timer.h"
#include "memory.h"
#include "cartridge.h"

typedef LR35902 CPU;

class Gameboy
{
    public:

        MCU mcu;
        LR35902 cpu;
        Timer timer;
        PPU ppu;
        std::unique_ptr<Cartridge> cartridge;
        
        // Constructor/Destructor //

        Gameboy();
        ~Gameboy();

        void reset();

        void step();

        void run();
        void pause();

        void load_rom(const char* filename);

    private:

        std::atomic<bool> exit;
        std::atomic<bool> running;

        std::thread run_thread;
        std::mutex  run_mutex;
        std::condition_variable run_cv;

        void run_loop();
};

#endif // _GAMEBOY_H_
