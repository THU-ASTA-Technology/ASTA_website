# Generated by Django 3.1.4 on 2020-12-16 23:17

from django.db import migrations, models
import django.utils.timezone


class Migration(migrations.Migration):

    dependencies = [
        ('database', '0030_auto_20201216_2315'),
    ]

    operations = [
        migrations.AddField(
            model_name='contest',
            name='timeUp',
            field=models.DateTimeField(default=django.utils.timezone.now),
        ),
        migrations.AlterField(
            model_name='contest',
            name='registerTimeUp',
            field=models.DateTimeField(default=django.utils.timezone.now),
        ),
    ]