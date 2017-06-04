# FVlib_OpenLog
Automated and protected logging sessions against file oversize and other problems
## Important Considerations ##
The sd must contain a "config.txt" file with the following lines:
> 9600,26,3,2,0,0,0
> baud,escape,esc#,mode,verb,echo,ignoreRX


findLastLoggingSession
returns:
- 0: Ok
- 1: Error
