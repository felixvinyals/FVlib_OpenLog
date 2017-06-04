# FVlib_OpenLog
Automated and protected logging sessions against file oversize and other problems
## Important Considerations ##
The sd must contain a "config.txt" file with the following lines:
> 9600,26,3,2,0,0,0
> baud,escape,esc#,mode,verb,echo,ignoreRX


findLastLoggingSession (String loggingFileName)
returns the last file free to log from 1 to 9. If return 255 no free file found found
