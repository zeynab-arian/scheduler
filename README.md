# Multi-Processor Task Scheduler

این برنامه یک شبیه‌ساز ساده برای برنامه‌ریزی وظایف چندپردازنده‌ای است. این کد به زبان C نوشته شده و از pthread برای مدیریت نخ‌ها و mutexها برای همگام‌سازی استفاده می‌کند.

- پشتیبانی از چندین پردازنده (NUM_PROCESSORS تعداد پردازنده‌ها در اینجا 3 است).
- مدیریت منابع مشترک (منابع R1، R2 و R3) بین وظایف.
- اجرای وظایف با توجه به زمان اجرا و دوره‌های مختلف.
- انتظار و مدیریت وظایف در صف‌های منتظر.

### ساختار کد:
- **main.c**: فایل اصلی که شامل تابع main و توابع اصلی برنامه است.
- **pthread.h, stdio.h, stdlib.h, ...**: کتابخانه‌های استفاده شده برای مدیریت نخ‌ها، ورودی/خروجی، حافظه، و ...
- بله، الان توابع اصلی کد شما را به صورت خلاصه توضیح می‌دهم:

1. **init_resources(int R1, int R2, int R3)**:
   - **ورودی**: مقادیر منابع R1، R2، R3.
   - **عملیات**: تنظیم مقادیر منابع بر اساس ورودی‌ها.

2. **bool allocate_resources(Task* task)**:
   - **ورودی**: یک اشاره‌گر به ساختار Task.
   - **خروجی**: مقدار منطقی (صحیح یا غلط) بر اساس موفقیت یا شکست در اختصاص منابع به وظیفه.
   - **عملیات**: بررسی وجود منابع کافی برای وظیفه و در صورت موجود بودن، منابع را اختصاص می‌دهد.

3. **void release_resources(Task* task)**:
   - **ورودی**: یک اشاره‌گر به ساختار Task.
   - **عملیات**: بازگرداندن منابع مورد استفاده توسط وظیفه به سیستم.

4. **void add_to_waiting_queue(Task* task)**:
   - **ورودی**: یک اشاره‌گر به ساختار Task.
   - **عملیات**: اضافه کردن وظیفه به صف انتظار برای انتظار در صورت نبود منابع کافی.

5. **Task* get_from_waiting_queue()**:
   - **خروجی**: یک اشاره‌گر به ساختار Task یا NULL اگر صف انتظار خالی باشد.
   - **عملیات**: برداشتن وظیفه از صف انتظار برای اختصاص به پردازنده‌ها.

6. **void* processor_thread(void* arg)**:
   - **ورودی**: یک اشاره‌گر به ساختار Processor.
   - **خروجی**: NULL.
   - **عملیات**: اجرای هر پردازنده به عنوان یک نخ جداگانه، مدیریت صف‌های آماده به کار، اختصاص منابع، اجرای وظایف و بازگرداندن منابع پس از اتمام.

7. **void read_input()**:
   - **عملیات**: خواندن ورودی‌ها از ورودی استاندارد، شامل تنظیم منابع و خواندن اطلاعات وظایف از ورودی.

8. **void print_deadlines()**:
   - **عملیات**: چاپ دوره‌های زمانی برای هر وظیفه بر اساس دوره و تعداد تکرارها.

9. **void init_processors()**:
   - **عملیات**: تنظیم و شروع هر پردازنده به عنوان یک نخ جدید برای اجرای همزمان وظایف.

10. **void* main_thread_function(void* arg)**:
    - **ورودی**: اشاره‌گر به چیزی (در این مورد NULL).
    - **خروجی**: NULL.
    - **عملیات**: اجرای نخ اصلی برنامه برای چاپ وضعیت منابع، صف انتظار وظایف، وضعیت پردازنده‌ها و مدیریت دسترسی‌ها.

11. **void start_main_thread()**:
    - **عملیات**: شروع اجرای نخ اصلی برنامه.



ورودی:
ورودی باید به شکل زیر باشد:
```
R1: <value> R2: <value> R3: <value>
<TaskName1> <Period> <ExecutionTime> <Resource1> <Resource2> <Resource3> <ProcessorID> <Repetitions>
<TaskName2> <Period> <ExecutionTime> <Resource1> <Resource2> <Resource3> <ProcessorID> <Repetitions>
...
```
برای مثال:
```
R1: 10 R2: 15 R3: 20
Task1 5 2 3 4 5 0 3
Task2 10 3 1 2 3 1 1
Task3 7 1 4 2 1 2 2
```

خروجی:
خروجی نمایش وضعیت منابع، صف انتظار وظایف، وضعیت پردازنده‌ها و ... در طول زمان را نمایش می‌دهد.




