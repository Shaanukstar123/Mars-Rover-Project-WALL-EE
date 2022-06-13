for i in range(255):
    print(i ,"mod 13", i % 13)
    print(-i, "mod 13", -i % 13)
    if i % 13 != 0:
        print(13-(i % 13) == -i % 13)
    else:
        print(i % 13 == -i % 13)