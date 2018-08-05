import matplotlib.dates as mdates
import matplotlib.pyplot as mplot
import datetime
import calendar

file=open("clog.txt","r")#opens file to read
data=file.read()#reads file into string
file.close()#closes file

data=data.split("\n")#splits string on every new line

while data.count('')>0:#while there are empty lines
    data.remove('')#removes empty line
 
temperatures=[]#data for y axis
for i in range(1,len(data),2):#reference every odd index
    temperatures.append(float(data[i]))#add data to y axis list
 
months=[]#creates list for referencing months
for i in range(1,13):#for each month in the year
    months.append(calendar.month_abbr[i])#copy names from library
    
dtime=[]#data for x axis formatted in datetime
for i in range(0,len(data),2):#for every even index
    preform=data[i].split(' ')#split date into year, time, day and month
    pretime=preform[4].split(':')#split time into hours, minutes and seconds
    dtime.append(datetime.datetime(#add each date and time into the data list
        int(preform[5]),#year
        months.index(preform[1]),#month referenced from list
        int(preform[3]),#day
        int(pretime[0]),#hour
        int(pretime[1]),#minute
        int(pretime[2])))#second

for i in range(0, len(dtime)):#for each date
    dtime[i]=mdates.date2num(dtime[i])#convert from datetime to matplot format
    
print("number of temperatures:"+str(len(temperatures)))
mplot.plot_date(dtime,temperatures)#plot the graph
mplot.show()#show the graph

#the following code is for tests and improvements
##diffavg=0
##for i in range(0, len(temperatures)-1):
##    diffavg+=abs(temperatures[i]-temperatures[i+1])
##
##diffavg/=len(temperatures)-1
##print(diffavg)


