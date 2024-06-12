"""
URL configuration for counter project.

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/4.2/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path
from myapp import views

urlpatterns = [
    path('admin/', admin.site.urls),
    path('', views.connect_ble_view, name='connect_ble'),
    path('push-up/', views.push_count, name='push_up'),
    path('sit-up/', views.sit_count, name='sit_up'),
    path('mode/', views.mode, name='mode'),
    path('connect-ble/', views.connect_ble_view, name='connect_ble'),
    path('get-counter/', views.get_counter, name='get_counter'),
    path('history/', views.history, name='history'),
    path('data-csv/', views.data_csv, name='data_csv'),
    path('add-data/', views.add_data_to_csv, name='add_data'),
    path('clear-history/', views.clear_csv, name='clear_csv')
]
